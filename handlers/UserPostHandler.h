//
// Created by dragoon on 8/21/17.
//

#ifndef EPOLL_TRY_USERPOSTHANDLER_H
#define EPOLL_TRY_USERPOSTHANDLER_H


#include "../Buffer.h"

namespace UserPostHandler {
    inline void process(Buffer *buffer) {
        char *buf = buffer->rdBuf;
        if (buf[12] == 110) {
            char *startPos = ::Util::getStartBodyIdx(buf + 36); // +24
            if (startPos - buf >= buffer->readCount) {
                buffer->writeBadRequest();
                return;
            }
            int fieldsCnt = 6;
            char *email = 0;
            char *firstName = 0;
            char *lastName = 0;
            char gender = 0;
            char *birthDate = 0;
            char *idStr = 0;
            char *tmp;
            startPos = Util::jsonFirstStringDelim(startPos);
            while (fieldsCnt != 0) {
                switch (*startPos) {
                    case 'e'://email
                        startPos += 8; //email: "
                        tmp = Util::advanceIfString(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        email = startPos + 1;
                        startPos = tmp;
                        break;
                    case 'f'://first name
                        startPos += 13; //first_name: "
                        tmp = Util::advanceIfString(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        firstName = startPos + 1;
                        startPos = tmp;
                        break;
                    case 'l'://last name
                        startPos += 12; //last_name: "
                        tmp = Util::advanceIfString(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        lastName = startPos + 1;
                        startPos = tmp;
                        break;
                    case 'g'://gender
                        if (*(startPos + 9) != '"') {
                            buffer->writeBadRequest();
                            return;
                        }
                        switch (*(startPos + 10)) {
                            case 'm':
                                gender = 'm';
                                break;
                            case 'f':
                                gender = 'f';
                                break;
                            default:
                                buffer->writeBadRequest();
                                return;
                        }
                        if (*(startPos + 11) != '"') {
                            buffer->writeBadRequest();
                            return;
                        }
                        startPos += 15;
                        break;
                    case 'b'://birth date
                        startPos += 13; //id":
                        tmp = Util::advanceIfInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        birthDate = startPos;
                        startPos = tmp;
                        break;
                    default:
                        startPos += 5; //id":
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        idStr = startPos;
                        startPos = tmp;
                }
                --fieldsCnt;
            }
            buffer->writePostOk();
            int id = Util::parseUInt(idStr);
            User *user = ::storage::users + id;
            user->id = id;
            *(user->email + Util::copyCharArray(email, user->email)) = 0;
            Util::unicodeDecode(firstName, user->firstName);
            Util::unicodeDecode(lastName, user->lastName);
            user->male = gender;
            user->birthDate = Util::parseInt(birthDate);
            user->updateUserOutput(buffer->rdBuf, buffer->smallBuf);
        } else {
            int userId = Util::tryParsePositiveIntPostPath(buf + 12); // 11 pos
            if (userId == INT_MIN) {
                buffer->writeNotFound();
                return;
            }
            User *user = userId < storage::usersArrayLength ? storage::users + userId : NULL;
            if (user == NULL || user->id == 0) {
                buffer->writeNotFound();
                return;
            }
            char *startPos = ::Util::getStartBodyIdx(buf + 34);
            if (startPos - buf >= buffer->readCount) {
                buffer->writeBadRequest();
                return;
            }
            char *email = 0;
            char *firstName = 0;
            char *lastName = 0;
            char gender = 0;
            char *birthDate = 0;
            char *tmp;
            startPos = Util::jsonFirstStringDelim(startPos);
            while (startPos - buf < buffer->readCount) {
                switch (*startPos) {
                    case 'e'://email
                        startPos += 8; //email: "
                        tmp = Util::advanceIfString(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        email = startPos + 1;
                        startPos = tmp;
                        break;
                    case 'f'://first name
                        startPos += 13; //first_name: "
                        tmp = Util::advanceIfString(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        firstName = startPos + 1;
                        startPos = tmp;
                        break;
                    case 'l'://last name
                        startPos += 12; //last_name: "
                        tmp = Util::advanceIfString(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        lastName = startPos + 1;
                        startPos = tmp;
                        break;
                    case 'g'://gender
                        if (*(startPos + 9) != '"') {
                            buffer->writeBadRequest();
                            return;
                        }
                        switch (*(startPos + 10)) {
                            case 'm':
                                gender = 'm';
                                break;
                            case 'f':
                                gender = 'f';
                                break;
                            default:
                                buffer->writeBadRequest();
                                return;
                        }
                        if (*(startPos + 11) != '"') {
                            buffer->writeBadRequest();
                            return;
                        }
                        startPos += 15;
                        break;
                    default://birth date
                        startPos += 13; //id":
                        tmp = Util::advanceIfInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        birthDate = startPos;
                        startPos = tmp;
                }
            }
            buffer->writePostOk();
            if (email != 0) {
                *(user->email + Util::copyCharArray(email, user->email)) = 0;
            }
            if (firstName != 0) {
                Util::unicodeDecode(firstName, user->firstName);
            }
            if (lastName != 0) {
                Util::unicodeDecode(lastName, user->lastName);
            }
            if (gender != 0) {
                user->male = gender;
            }
            if (birthDate != 0) {
                user->birthDate = Util::parseInt(birthDate);
            }
            user->updateUserOutput(buffer->rdBuf, buffer->smallBuf);
        }
    }

};


#endif //EPOLL_TRY_USERPOSTHANDLER_H
