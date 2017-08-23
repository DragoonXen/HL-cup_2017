//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_LOCATIONGETHANDLER_H
#define EPOLL_TRY_LOCATIONGETHANDLER_H


#include "../Buffer.h"
#include "../model/Location.h"

class Buffer;

namespace LocationGetHandler {

    void process(Buffer *buf);

    void writeAvgOutput(Buffer *buffer, size_t sum, size_t count);
};


#endif //EPOLL_TRY_LOCATIONGETHANDLER_H
