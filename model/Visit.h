//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_VISITS_H
#define EPOLL_TRY_VISITS_H

#include <cstddef>
#include "../Util.h"
#include "../Const.h"

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

    inline void updateVisitOutput(char *tempBuf, char *smallBuf) {
        char *tempAnsBuffer = tempBuf;

        tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_ID, tempAnsBuffer);
        tempAnsBuffer += ::Util::uintToStringBytes(this->id, tempAnsBuffer, smallBuf);

        tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_LOCATION, tempAnsBuffer);
        tempAnsBuffer += ::Util::uintToStringBytes(this->location, tempAnsBuffer, smallBuf);

        tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_USER, tempAnsBuffer);
        tempAnsBuffer += ::Util::uintToStringBytes(this->user, tempAnsBuffer, smallBuf);

        tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_VISITED_AT, tempAnsBuffer);
        tempAnsBuffer += ::Util::intToStringBytes(this->visitedAt, tempAnsBuffer, smallBuf);

        tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_MARK_AND_STOP, tempAnsBuffer);
        *(tempAnsBuffer - 2) = (char) (this->mark + '0');
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

#endif //EPOLL_TRY_VISITS_H
