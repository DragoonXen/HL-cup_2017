//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_ROUTING_H
#define EPOLL_TRY_ROUTING_H

#include "Util.h"
#include "Const.h"
#include "Buffer.h"
#include "handlers/UserGetHandler.h"
#include "handlers/LocationGetHandler.h"
#include "handlers/VisitGetHandler.h"
#include "handlers/LocationPostHandler.h"
#include "handlers/UserPostHandler.h"
#include "handlers/VisitPostHandler.h"

namespace Routing {
    void process(Buffer *buf);
};


#endif //EPOLL_TRY_ROUTING_H
