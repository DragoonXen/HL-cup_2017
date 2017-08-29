//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_USER_H
#define EPOLL_TRY_USER_H

#include "Visit.h"

class User {
    static const int MAX_BUF_SIZE = 389;

public:
    char getBuffer[MAX_BUF_SIZE];
    size_t getSize;

    int id;
    char email[201]; // 100 symb
    char firstName[101]; // 50 chars
    char lastName[101];// 50 chars
    char male;
    int birthDate;
    bool sorted = false;
    std::vector<Visit *> visits;

    inline User &operator=(const User &user) {
        this->id = user.id;
        this->male = user.male;
        this->birthDate = user.birthDate;
        Util::copyCharArray(user.email, this->email);
        Util::copyCharArray(user.firstName, this->firstName);
        Util::copyCharArray(user.lastName, this->lastName);

        return *this;
    }

    inline void updateUserOutput(char *tempBuf, char *smallBuf) {
        char *tempAnsBuffer = tempBuf;
        tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_ID, tempAnsBuffer);
        tempAnsBuffer += ::Util::uintToStringBytes(this->id, tempAnsBuffer, smallBuf);

        tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_EMAIL, tempAnsBuffer);
        tempAnsBuffer += ::Util::copyCharArray(this->email, tempAnsBuffer);

        tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_FIRST_NAME, tempAnsBuffer);
        tempAnsBuffer += ::Util::copyCharArray(this->firstName, tempAnsBuffer);

        tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_LAST_NAME, tempAnsBuffer);
        tempAnsBuffer += ::Util::copyCharArray(this->lastName, tempAnsBuffer);

        tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_GENDER_AND_BD, tempAnsBuffer);
        *(tempAnsBuffer - Const::USER_FORMAT_GENDER_AND_BD_SZ + 13) = this->male;
        tempAnsBuffer += ::Util::intToStringBytes(this->birthDate, tempAnsBuffer, smallBuf);
        *(tempAnsBuffer++) = '}';
        *tempAnsBuffer = 0;// for copy char array

        char *writeBuf = this->getBuffer;
        writeBuf += ::Util::copyCharArray(Const::OK_PREPARED, writeBuf);
        writeBuf += ::Util::uintToStringBytes((int) (tempAnsBuffer - tempBuf), writeBuf, smallBuf);
        writeBuf += ::Util::copyCharArray(Const::OK_PREPARED_SECOND, writeBuf);
        writeBuf += ::Util::copyCharArray(tempBuf, writeBuf);
//    *writeBuf = 0;
        this->getSize = writeBuf - this->getBuffer;
    }
};

#endif //EPOLL_TRY_USER_H
