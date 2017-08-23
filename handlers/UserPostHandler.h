//
// Created by dragoon on 8/21/17.
//

#ifndef EPOLL_TRY_USERPOSTHANDLER_H
#define EPOLL_TRY_USERPOSTHANDLER_H


#include "../Buffer.h"

class UserPostHandler {
public:
    static void process(Buffer *buffer);
};


#endif //EPOLL_TRY_USERPOSTHANDLER_H
