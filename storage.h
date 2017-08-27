//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_STORAGE_H
#define EPOLL_TRY_STORAGE_H

#include "model/User.h"
#include "model/Visit.h"
#include "model/Location.h"

namespace storage {
    extern User *users;
    extern Location *locations;
    extern Visit *visits;

    const size_t usersArrayLength = 1024*1024;
    const size_t locationsArrayLength = 900000;
    const size_t visitsArrayLength = 10243250;//1024 * 1024 * 11;
//    const size_t usersArrayLength = 200 * 1024 * 10;
//    const size_t locationsArrayLength = 200 * 1024 * 10;
//    const size_t visitsArrayLength = 1024 * 1024 * 2 * 10;

    void printCnt();
};


#endif //EPOLL_TRY_STORAGE_H
