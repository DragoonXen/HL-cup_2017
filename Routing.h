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
        int *ptr = (int *) buf->rdBuf;
        int val = (*ptr ^ *(ptr + 1));
        //1160196200 get use
        //1127950696 get loc
        //1396519784 get visit
        //994009200 post loc
        //656826480 post user
        //1025859696 post visit
        if (val > 1025859696) {
            buf->closeConnection = false;//get
            if (val == 1160196200) {
                UserGetHandler::process(buf);
            } else if (val == 1127950696) {
                LocationGetHandler::process(buf);
            } else {
                VisitGetHandler::process(buf);
            }
        } else {
            buf->closeConnection = true;//post
            if (val == 1025859696) {
                VisitPostHandler::process(buf);
            } else if (val == 656826480) {
                UserPostHandler::process(buf);
            } else {
                LocationPostHandler::process(buf);
            }
        }
    }
};


#endif //EPOLL_TRY_ROUTING_H
