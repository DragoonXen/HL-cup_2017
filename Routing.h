//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_ROUTING_H
#define EPOLL_TRY_ROUTING_H

#include "Buffer.h"
#include "handlers/UserGetHandler.h"
#include "handlers/LocationGetHandler.h"
#include "handlers/VisitGetHandler.h"
#include "handlers/LocationPostHandler.h"
#include "handlers/UserPostHandler.h"
#include "handlers/VisitPostHandler.h"

namespace Routing {
    inline void process(Buffer *buf) {
        char *buffer = buf->rdBuf;
//    buf->writeNotFound();
        if (buffer[0] == 'G') {
            buf->closeConnection = false;
            if (buffer[5] == 117) {
                UserGetHandler::process(buf);
            } else if (buffer[5] == 108) {
                LocationGetHandler::process(buf);
            } else {
                VisitGetHandler::process(buf);
            }
        } else {
            buf->closeConnection = true;
            if (buffer[6] == 117) {
                UserPostHandler::process(buf);
            } else if (buffer[6] == 118) {
                VisitPostHandler::process(buf);
            } else {
                LocationPostHandler::process(buf);
            }
        }
    }
};


#endif //EPOLL_TRY_ROUTING_H
