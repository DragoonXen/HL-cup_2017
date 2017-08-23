#include "User.h"

//
// Created by dragoon on 8/20/17.
//
User &User::operator=(const User &user) {
    this->id = user.id;
    this->male = user.male;
    this->birthDate = user.birthDate;
    Util::copyCharArray(user.email, this->email);
    Util::copyCharArray(user.firstName, this->firstName);
    Util::copyCharArray(user.lastName, this->lastName);

    return *this;
}

void User::updateUserOutput(Buffer *buffer) {
    char *tempAnsBuffer = buffer->rdBuf;
    tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_ID, tempAnsBuffer);
    tempAnsBuffer += ::Util::uintToStringBytes(this->id, tempAnsBuffer, buffer->smallBuf);

    tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_EMAIL, tempAnsBuffer);
    tempAnsBuffer += ::Util::copyCharArray(this->email, tempAnsBuffer);

    tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_FIRST_NAME, tempAnsBuffer);
    tempAnsBuffer += ::Util::copyCharArray(this->firstName, tempAnsBuffer);

    tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_LAST_NAME, tempAnsBuffer);
    tempAnsBuffer += ::Util::copyCharArray(this->lastName, tempAnsBuffer);

    tempAnsBuffer += ::Util::copyCharArray(Const::USER_FORMAT_GENDER_AND_BD, tempAnsBuffer);
    *(tempAnsBuffer - Const::USER_FORMAT_GENDER_AND_BD_SZ + 13) = this->male;
    tempAnsBuffer += ::Util::intToStringBytes(this->birthDate, tempAnsBuffer, buffer->smallBuf);
    *(tempAnsBuffer++) = '}';
    *tempAnsBuffer = 0;// for copy char array

    char *writeBuf = this->getBuffer;
    writeBuf += ::Util::copyCharArray(Const::OK_PREPARED, writeBuf);
    writeBuf += ::Util::uintToStringBytes((int) (tempAnsBuffer - buffer->rdBuf), writeBuf, buffer->smallBuf);
    writeBuf += ::Util::copyCharArray(Const::OK_PREPARED_SECOND, writeBuf);
    writeBuf += ::Util::copyCharArray(buffer->rdBuf, writeBuf);
//    *writeBuf = 0;
    this->getSize = writeBuf - this->getBuffer;
}
