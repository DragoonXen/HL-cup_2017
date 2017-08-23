//
// Created by dragoon on 8/20/17.
//
#include "Location.h"

Location &Location::operator=(const Location &other) {
    this->id = other.id;
    this->place = other.place;
    this->distance = other.distance;
    Util::copyCharArray(other.country, this->country);
    Util::copyCharArray(other.city, this->city);

    return *this;
}

void Location::updateLocationOutput(Buffer *buffer) {
    char *tempAnsBuffer = buffer->rdBuf;

    tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_ID, tempAnsBuffer);
    tempAnsBuffer += ::Util::uintToStringBytes(this->id, tempAnsBuffer, buffer->smallBuf);

    tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_PLACE, tempAnsBuffer);
    tempAnsBuffer += ::Util::copyCharArray(this->place.c_str(), tempAnsBuffer);

    tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_COUNTRY, tempAnsBuffer);
    tempAnsBuffer += ::Util::copyCharArray(this->country, tempAnsBuffer);

    tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_CITY, tempAnsBuffer);
    tempAnsBuffer += ::Util::copyCharArray(this->city, tempAnsBuffer);

    tempAnsBuffer += ::Util::copyCharArray(Const::LOCATION_FORMAT_DISTANCE, tempAnsBuffer);
    tempAnsBuffer += ::Util::uintToStringBytes(this->distance, tempAnsBuffer, buffer->smallBuf);
    *tempAnsBuffer++ = '}';
    *tempAnsBuffer = 0;

    char *writeBuf = this->getBuffer;
    writeBuf += ::Util::copyCharArray(Const::OK_PREPARED, writeBuf);
    writeBuf += ::Util::uintToStringBytes((int) (tempAnsBuffer - buffer->rdBuf), writeBuf, buffer->smallBuf);
    writeBuf += ::Util::copyCharArray(Const::OK_PREPARED_SECOND, writeBuf);
    writeBuf += ::Util::copyCharArray(buffer->rdBuf, writeBuf);
//    *writeBuf = 0;
    this->getSize = writeBuf - this->getBuffer;
}
