#include "fileReade.h"
#include "Routing.h"
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <thread>
#include <sys/mman.h>

#define MAXEVENTS 64

constexpr int recvBuf = 1024 * 4;
constexpr int sendBuf = 1024 * 16;

static int make_socket_nodelay(int sfd) {
    int flags;

    flags = 1;
    if (setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flags, sizeof(int)) < 0) {
        perror("setsockopt");
        return -1;
    }
    if (setsockopt(sfd, IPPROTO_TCP, TCP_QUICKACK, (char *) &flags, sizeof(int)) < 0) {
        perror("setsockopt");
        return -1;
    }
//    if (setsockopt(sfd, SOL_SOCKET, SO_DONTROUTE, (char *) &flags, sizeof(int)) < 0) {
//        perror("setsockopt");
//        return -1;
//    }
//    if (setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, &sendBuf, sizeof(sendBuf)) < 0) {
//        perror("setsockopt");
//        return -1;
//    }
//    if (setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &recvBuf, sizeof(recvBuf)) < 0) {
//        perror("setsockopt");
//        return -1;
//    }

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
    hints.ai_family = PF_UNSPEC;     /* Return IPv4 and IPv6 choices */
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

Buffer Buffer::instance[];

//[BUFFERS_COUNT];

//uint32_t availableBuffers[BUFFERS_COUNT];
//int currBuffersTop;

void processThread(int epollDescriptor, int sfd, int idx, epoll_event *events) {
    Buffer *currentBuffer = Buffer::instance + idx;

    while (1) {
//        std::cout << "listen to epoll instance " << epollDescriptor << std::endl;
        int n = epoll_wait(epollDescriptor, events, MAXEVENTS, 0);
        int i, e;
        if (n) {
//            std::cout << "thread " << idx << " received " << n << " events" << std::endl;
        }
        for (i = 0; i < n; i++) {
            e = events[i].events;
            if ((e & EPOLLERR) || (e & EPOLLHUP) || (e & EPOLLRDHUP) || (!(e & EPOLLIN) && !(e & EPOLLOUT))) {
//                std::cout << "close conn by thread " << idx << " received" << std::endl;
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
//                        if ((errno == EAGAIN) ||
//                            (errno == EWOULDBLOCK)) {
//                            /* We have processed all incoming
//                               connections. */
//                            break;
//                        } else {
//                            perror("accept");
                        break;
//                        }
                    }
//                    fprintf(stdout, "connection %d\n", infd);
                    make_socket_nodelay(infd);
//                    std::cout << "open conn by thread " << idx << " received, infd " << infd << std::endl;
                    events[newEvents].data.fd = infd;
//                    *(&events[newEvents].data.u32 + 1) = NO_BUFFER;
                    events[newEvents].events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLET;
//                    fprintf(stdout, "%d\n", events[newEvents].events);
                    epoll_ctl(epollDescriptor, EPOLL_CTL_ADD, infd, &events[newEvents++]);
                }
                // new connection here
            } else if (e & 1) { // ready to read_to_buf
                currentBuffer->source = events[i].data.fd;
                if (currentBuffer->processRequest()) {
                    Routing::process(currentBuffer);
                }
//                std::cout << "read bytes " << currentBuffer->readCount << std::endl;
            }
        }
    }
}

int main(int argc, char *argv[]) {

//    int which = PRIO_PROCESS;
//    int ret;
//    ret = setpriority(which, getpid(), -20);
//    std::cout << ret << std::endl;

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
    for (int i = 0; i != THREADS_COUNT; ++i) {
        ::Util::copyCharArray(Const::OK_PREPARED, Buffer::instance[i].wrBuf);
        ::Util::copyCharArray(Const::AVG_FORMAT, Buffer::instance[i].avgFormat);
    }
    for (int i = 1; i < THREADS_COUNT; ++i) {
        struct epoll_event *threadEvents = (epoll_event *) calloc(MAXEVENTS, sizeof mainEvent);
        std::cout << "thread " << i << " started" << std::endl;
        new std::thread(processThread, efd, sfd, i, threadEvents);
//        thread.detach();
    }
    std::cout << "memlock " << mlockall(MCL_CURRENT | MCL_FUTURE) << std::endl;
    processThread(efd, sfd, 0, events);
//    for (int i = 0; i != BUFFERS_COUNT; ++i) {
//        buffers[i].idx = i;
//        availableBuffers[currBuffersTop++] = i;
//    }

    /* The event loop */

    free(events);

    close(sfd);

    return EXIT_SUCCESS;
}

