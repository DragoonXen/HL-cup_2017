//
// Created by dragoon on 8/20/17.
//
#include "Routing.h"

void Routing::process(Buffer *buf) {
    char *buffer = buf->rdBuf;
//    buf->writeNotFound();
    if (buffer[0] == 'G') {
        buf->closeConnection = false;
        switch (buffer[5]) {
            case 117://users
                UserGetHandler::process(buf);
                break;
            case 108:
                LocationGetHandler::process(buf);
                break;
            default:
                VisitGetHandler::process(buf);
        }
    } else {
        buf->closeConnection = true;
        switch (buffer[6]) {
            case 117://users
                UserPostHandler::process(buf);
                break;
            case 118://visits
                VisitPostHandler::process(buf);
                break;
            default://locations 108
                LocationPostHandler::process(buf);
        }
    }
}