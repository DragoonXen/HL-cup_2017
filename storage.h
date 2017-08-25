//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_STORAGE_H
#define EPOLL_TRY_STORAGE_H

#include <algorithm>
#include "model/User.h"
#include "model/Visit.h"
#include "model/Location.h"

class User;

class Location;

class Visit;

class Buffer;

namespace storage {
    extern User *users;
    extern Location *locations;
    extern Visit *visits;

    const size_t usersArrayLength = 200 * 1024 * 10;
    const size_t locationsArrayLength = 200 * 1024 * 10;
    const size_t visitsArrayLength = 1024 * 1024 * 2 * 10;

    void printCnt();
};


#endif //EPOLL_TRY_STORAGE_H
