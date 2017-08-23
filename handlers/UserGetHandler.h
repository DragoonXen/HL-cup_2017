//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_USERGETHANDLER_H
#define EPOLL_TRY_USERGETHANDLER_H


#include "../Buffer.h"
#include "../model/User.h"
#include <climits>
#include <cstdio>
#include "../storage.h"
#include <list>
#include <algorithm>

class Buffer;

class Visit;

class User;

namespace UserGetHandler {

    void process(Buffer *buf);

    void writeResponse(Buffer *buffer, const std::vector<Visit *> &vc);

    char *formatVisit(char *buffer, const Visit *visit, char *smallBuf);
};


#endif //EPOLL_TRY_USERGETHANDLER_H
