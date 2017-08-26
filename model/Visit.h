//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_VISITS_H
#define EPOLL_TRY_VISITS_H


#include "Location.h"

class Buffer;

class Visit {
    static const int MAX_BUF_SIZE = 189;
public:
    char getBuffer[MAX_BUF_SIZE];
    size_t getSize;

    int id;
    int location;
    int user;
    int visitedAt;
    int mark;

    void updateVisitOutput(Buffer *buffer);
};

#endif //EPOLL_TRY_VISITS_H
