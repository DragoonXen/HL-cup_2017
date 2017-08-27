#include "fileReade.h"
#include "Routing.h"
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <thread>
#include <sys/mman.h>

#define MAXEVENTS 32

//constexpr int recvBuf = 1024 * 4;
//constexpr int sendBuf = 1024 * 16;
#define BUF_COUNT 128

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

static int create_and_bind(char *port) {
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

//[BUFFERS_COUNT];

//uint32_t availableBuffers[BUFFERS_COUNT];
//int currBuffersTop;

void processThread(int epollDescriptor, int idx, epoll_event *events) {

    Buffer buffers[BUF_COUNT];
    uint32_t availableBuffers[BUF_COUNT];
    int currBuffersTop = 0;
    for (uint32_t i = 0; i != BUF_COUNT; ++i) {
        availableBuffers[currBuffersTop++] = i;
        ::Util::copyCharArray(Const::OK_PREPARED, buffers[i].wrBuf);
        ::Util::copyCharArray(Const::AVG_FORMAT, buffers[i].avgFormat);
    }

    /* The event loop */
    while (1) {
        int n, i;

        n = epoll_wait(epollDescriptor, events, MAXEVENTS, 0);
        int e;
        for (i = n - 1; i >= 0; --i) {
            e = events[i].events;
            if ((e & EPOLLERR) || (e & EPOLLHUP) || (e & EPOLLRDHUP) || (!(e & EPOLLIN) && !(e & EPOLLOUT))) {
//                std::cout << e << " thread " << idx << " close " << events[i].data.fd << std::endl;
                close(events[i].data.fd); // close connection
                continue;
            }
//            std::cout << "thread " << idx << " read " << events[i].data.fd << std::endl;
            if (e & 1) {
                //                    std::cout << "main thread read " << events[i].data.fd << std::endl;
                uint32_t bufNom = availableBuffers[--currBuffersTop];
                if (currBuffersTop == -1) {
                    std::cout << "alert! negative buffer!" << std::endl;
                }
                *(&events[i].data.u32 + 1) = bufNom;
                buffers[bufNom].source = events[i].data.fd;
                buffers[bufNom].readSource();
                Routing::process(buffers + bufNom);
                events[i].events = EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
                epoll_ctl(epollDescriptor, EPOLL_CTL_MOD, events[i].data.fd, events + i);
//                if (s == -1) {
//                    perror("epoll_ctl");
//                    abort();
//                }
            } else if (e & 4) { // ready to read_to_buf
//                std::cout << "main thread write " << events[i].data.fd << std::endl;
                uint32_t bufNom = *(&events[i].data.u32 + 1);
                buffers[bufNom].writeResponse();
                availableBuffers[currBuffersTop++] = bufNom;
//                *(&events[i].data.u32 + 1) = NO_BUFFER;
                if (buffers[bufNom].closeConnection) {
                    close(events[i].data.fd);
                } else { // keep alive, wait for response
                    events[i].events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
                    epoll_ctl(epollDescriptor, EPOLL_CTL_MOD, events[i].data.fd, events + i);
//                    if (s == -1) {
//                        perror("epoll_ctl");
//                        abort();
//                    }
                }
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
    mlockall(MCL_CURRENT | MCL_FUTURE);
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

    events = (epoll_event *) calloc(MAXEVENTS, sizeof mainEvent);
    int threadDescriptors[THREADS_COUNT];
    threadDescriptors[0] = efd;
    for (int i = 1; i < THREADS_COUNT; ++i) {
        int threadEfd = epoll_create1(EPOLL_CLOEXEC);
        std::cout << "epoll instance " << threadEfd << " created" << std::endl;
        if (threadEfd == -1) {
            perror("epoll_create");
            abort();
        }
        threadDescriptors[i] = threadEfd;
        struct epoll_event *threadEvents = (epoll_event *) calloc(MAXEVENTS, sizeof mainEvent);
        std::cout << "thread " << i << " started" << std::endl;
        new std::thread(processThread, threadEfd, i, threadEvents);
    }

    Buffer buffers[BUF_COUNT];

    uint32_t availableBuffers[BUF_COUNT];
    int currBuffersTop = 0;
    for (uint32_t i = 0; i != BUF_COUNT; ++i) {
        availableBuffers[currBuffersTop++] = i;
        ::Util::copyCharArray(Const::OK_PREPARED, buffers[i].wrBuf);
        ::Util::copyCharArray(Const::AVG_FORMAT, buffers[i].avgFormat);
    }

    /* The event loop */
    while (1) {
        int n, i;

        n = epoll_wait(efd, events, MAXEVENTS, 0);
        int e;
        for (i = n - 1; i >= 0; --i) {
            e = events[i].events;
            if ((e & EPOLLERR) || (e & EPOLLHUP) || (e & EPOLLRDHUP) || (!(e & EPOLLIN) && !(e & EPOLLOUT))) {
//                std::cout << e << " main thread close " << events[i].data.fd << std::endl;
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
                    make_socket_nodelay(infd);

                    events[newEvents].data.fd = infd;
//                    *(&events[newEvents].data.u32 + 1) = NO_BUFFER;
                    events[newEvents].events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
                    epoll_ctl(threadDescriptors[infd % THREADS_COUNT], EPOLL_CTL_ADD, infd, &events[newEvents++]);
//                    if (s == -1) {
//                        perror("epoll_ctl");
//                        abort();
//                    }
                }
                // new connection here
            } else {
                if (e & 1) {
                    //                    std::cout << "main thread read " << events[i].data.fd << std::endl;
                    uint32_t bufNom = availableBuffers[--currBuffersTop];
                    if (currBuffersTop == -1) {
                        std::cout << "alert! negative buffer!" << std::endl;
                    }
                    *(&events[i].data.u32 + 1) = bufNom;
                    buffers[bufNom].source = events[i].data.fd;
                    buffers[bufNom].readSource();
                    Routing::process(buffers + bufNom);
                    events[i].events = EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
                    epoll_ctl(efd, EPOLL_CTL_MOD, events[i].data.fd, events + i);
//                    if (s == -1) {
//                        perror("epoll_ctl");
//                        abort();
//                    }
                } else if (e & 4) { // ready to read_to_buf
//                std::cout << "main thread write " << events[i].data.fd << std::endl;
                    uint32_t bufNom = *(&events[i].data.u32 + 1);
                    buffers[bufNom].writeResponse();
                    availableBuffers[currBuffersTop++] = bufNom;
//                *(&events[i].data.u32 + 1) = NO_BUFFER;
                    if (buffers[bufNom].closeConnection) {
                        close(events[i].data.fd);
                    } else { // keep alive, wait for response
                        events[i].events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
                        epoll_ctl(efd, EPOLL_CTL_MOD, events[i].data.fd, events + i);
//                    if (s == -1) {
//                        perror("epoll_ctl");
//                        abort();
//                    }
                    }
                }
            }
        }
    }

    free(events);

    close(sfd);

    return EXIT_SUCCESS;
}

