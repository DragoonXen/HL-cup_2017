//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_LOCATION_H
#define EPOLL_TRY_LOCATION_H

#include <string>
#include <vector>
#include "../Util.h"
#include "../Buffer.h"

class Visit;

class Buffer;

class Location {
    static const int MAX_BUF_SIZE = 400;
public:
    char getBuffer[MAX_BUF_SIZE];
    size_t getSize;

    int id;
    char place[101];//unlimited
    char country[101];//50
    char city[101];//50
    int distance;
    std::vector<Visit *> visits;

    void updateLocationOutput(Buffer *buffer);

    Location &operator=(const Location &other);
};

#endif //EPOLL_TRY_LOCATION_H
