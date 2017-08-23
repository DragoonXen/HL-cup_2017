#include "Buffer.h"
#include "fileReade.h"
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>

#define MAXEVENTS 512
#define BUFFERS_COUNT 512
#define NO_BUFFER 1024

static int make_socket_nodelay(int sfd) {
    int flags;

    flags = 1;
    int s = setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flags, sizeof(int));
    if (s < 0) {
        perror("setsockopt");
        return -1;
    }

    return 0;
}

static int
make_socket_non_blocking(int sfd) {
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl");
        return -1;
    }

    return 0;
}

static int
create_and_bind(char *port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */

    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            /* We managed to bind successfully! */
            break;
        }

        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}

Buffer Buffer::instance;

//[BUFFERS_COUNT];

//uint32_t availableBuffers[BUFFERS_COUNT];
//int currBuffersTop;

int main(int argc, char *argv[]) {
    std::cout << "start program" << std::endl;
    fileReade::readData(argc < 3 ? "/root/" : argv[2]);
    int sfd, s;
    int efd;
    struct epoll_event mainEvent;
    struct epoll_event *events;
    {
        char *port = (char *) "80";
        if (argc > 1) {
            port = argv[1];
        }
        sfd = create_and_bind(port);
    }
    if (sfd == -1)
        abort();

    s = make_socket_non_blocking(sfd);
    if (s == -1)
        abort();

    s = listen(sfd, SOMAXCONN);
    if (s == -1) {
        perror("listen");
        abort();
    }

    efd = epoll_create1(EPOLL_CLOEXEC);
    if (efd == -1) {
        perror("epoll_create");
        abort();
    }

    mainEvent.data.fd = sfd;
    mainEvent.events = EPOLLIN | EPOLLET;
    s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &mainEvent);
    if (s == -1) {
        perror("epoll_ctl");
        abort();
    }

    /* Buffer where events are returned */
    events = (epoll_event *) calloc(MAXEVENTS, sizeof mainEvent);
//    char answer[] = "HTTP/1.1 404 Not Found\r\nConnection: Keep-Alive\r\nContent-Length: 0\r\nContent-Type: application/json\r\n\r\n";
//    char answer[] = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 2\r\nServer: Custom\r\nContent-Type: application/json\r\n\r\n{}\r\n";
//    int error;
//    socklen_t errlen;

    // init buffers
//    currBuffersTop = 0;
//    for (int i = BUFFERS_COUNT - 1; i >= 0; --i) {
//        buffers[i].idx = i;
//        ::Util::copyCharArray(Const::OK_PREPARED, buffers[i].wrBuf);
//        availableBuffers[currBuffersTop++] = i;
//    }
    ::Util::copyCharArray(Const::OK_PREPARED, Buffer::instance.wrBuf);
//    for (int i = 0; i != BUFFERS_COUNT; ++i) {
//        buffers[i].idx = i;
//        availableBuffers[currBuffersTop++] = i;
//    }

    /* The event loop */
    while (1) {
        int n, i;

        n = epoll_wait(efd, events, MAXEVENTS, -1);
        int e;
//        fprintf(stdout, "%d events received\n", n);
        for (i = 0; i < n; i++) {
            e = events[i].events;
//            fprintf(stdout, "event %d, id %d\n", e, events[i].data.fd);
            if ((e & EPOLLERR) || (e & EPOLLHUP) || (e & EPOLLRDHUP) || (!(e & EPOLLIN) && !(e & EPOLLOUT))) {
//                if (*(&events[i].data.u32 + 1) & ~NO_BUFFER) { //unfortenatly error caused, but buffer used
//                    uint32_t bufNom = *(&events[i].data.u32 + 1);
//                    availableBuffers[currBuffersTop++] = bufNom;
//                    *(&events[i].data.u32 + 1) = NO_BUFFER;
//                    fprintf(stdout, "closeConnection while buffer %d in use %d\n", bufNom, events[i].data.fd);
//                }
                std::cout << "close connection by event" << std::endl;
                close(events[i].data.fd); // close connection
                continue;
            }
            if (sfd == events[i].data.fd) {
                struct sockaddr in_addr;
                socklen_t in_len;
                int infd;
                in_len = sizeof in_addr;
                int newEvents = n;
                while (1) {
                    infd = accept4(sfd, &in_addr, &in_len, SOCK_NONBLOCK);
                    if (infd == -1) {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK)) {
                            /* We have processed all incoming
                               connections. */
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }
//                    fprintf(stdout, "connection %d\n", infd);
                    make_socket_nodelay(infd);

                    events[newEvents].data.fd = infd;
                    *(&events[newEvents].data.u32 + 1) = NO_BUFFER;
                    events[newEvents].events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
//                    fprintf(stdout, "%d\n", events[newEvents].events);
                    s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &events[newEvents++]);
                    if (s == -1) {
                        perror("epoll_ctl");
                        abort();
                    }
                }
                // new connection here
            } else {
                if (e & 1) { // ready to read_to_buf
//                    int count = read(events[i].data.fd, readBuf, sizeof readBuf);
//                    uint32_t bufNom;
//                    if (*(&events[i].data.u32 + 1) & NO_BUFFER) {
//                        // receive Buffer for request processing
//                        bufNom = availableBuffers[--currBuffersTop];
//                        *(&events[i].data.u32 + 1) = bufNom;
//                        buffers[bufNom].source = events[i].data.fd;
////                        buffers[bufNom].closeConnection = true;
////                        fprintf(stdout, "source %d use Buffer %d for read\n", events[i].data.fd, bufNom);
//                    } else {
//                        bufNom = *(&events[i].data.u32 + 1);
////                        fprintf(stdout, "source %d use Buffer %d for read\n", events[i].data.fd, bufNom);
//                    }
                    Buffer::instance.source = events[i].data.fd;
                    Buffer::instance.processRequest();
//                    if (!Buffer::instance.complete()) {
//                        Buffer::instance.writeResponse()
//                        if (Buffer::instance.closeConnection) {
//                            close(events[i].data.fd);
//                        } else { // keep alive, wait for response
//                            events[i].events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
//                            s = epoll_ctl(efd, EPOLL_CTL_MOD, events[i].data.fd, events + i);
//                            if (s == -1) {
//                                perror("epoll_ctl");
//                                abort();
//                            }
//                        }
//                    }
//                    if (count == -1) {
//                        if (errno != EAGAIN) {
//                            perror("read_to_buf");
//                            close(events[i].data.fd); // close connection
//                        }
//                        break;
//                    } else if (count == 0) {
//                        /* End of file. The remote has closed the
//                           connection. */
//                        close(events[i].data.fd); // close connection
//                        break;
//                    }
                    /* Write the Buffer to standard output */
//                    s = write(1, readBuf, count);
//                    if (s == -1) {
//                        perror("write");
//                        abort();
//                    }
                }
            }
        }
    }

    free(events);

    close(sfd);

    return EXIT_SUCCESS;
}

