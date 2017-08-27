//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_LOCATION_H
#define EPOLL_TRY_LOCATION_H

#include "Visit.h"

class Location {
    static const int MAX_BUF_SIZE = 400;
public:
    char getBuffer[MAX_BUF_SIZE];
    size_t getSize;

    int id;
    char place[101];//unlimited
    char country[101];//50
    int countryHash;
    char city[101];//50
    int distance;
    std::vector<Visit *> visits;

    Location &operator=(const Location &other) {
        this->id = other.id;
//    this->place = other.place;
        this->distance = other.distance;
        Util::copyCharArray(other.place, this->place);
        Util::copyCharArray(other.country, this->country);
        Util::copyCharArray(other.city, this->city);
        this->countryHash = other.countryHash;
        return *this;
    }

    void updateLocationOutput(char *tempBuf, char *smallBuf) {
        char *tempAnsBuffer = tempBuf;

        tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_ID, tempAnsBuffer);
        tempAnsBuffer += ::Util::uintToStringBytes(this->id, tempAnsBuffer, smallBuf);

        tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_PLACE, tempAnsBuffer);
        tempAnsBuffer += ::Util::copyCharArray(this->place, tempAnsBuffer);

        tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_COUNTRY, tempAnsBuffer);
        tempAnsBuffer += ::Util::copyCharArray(this->country, tempAnsBuffer);

        tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_CITY, tempAnsBuffer);
        tempAnsBuffer += ::Util::copyCharArray(this->city, tempAnsBuffer);

        tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_DISTANCE, tempAnsBuffer);
        tempAnsBuffer += ::Util::uintToStringBytes(this->distance, tempAnsBuffer, smallBuf);
        *tempAnsBuffer++ = '}';
        *tempAnsBuffer = 0;

        char *writeBuf = this->getBuffer;
        writeBuf += ::Util::copyCharArray(Const::OK_PREPARED, writeBuf);
        writeBuf += ::Util::uintToStringBytes((int) (tempAnsBuffer - tempBuf), writeBuf, smallBuf);
        writeBuf += ::Util::copyCharArray(Const::OK_PREPARED_SECOND, writeBuf);
        writeBuf += ::Util::copyCharArray(tempBuf, writeBuf);
//    *writeBuf = 0;
        this->getSize = writeBuf - this->getBuffer;
    }
};

#endif //EPOLL_TRY_LOCATION_H
