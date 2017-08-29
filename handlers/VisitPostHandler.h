//
// Created by dragoon on 8/21/17.
//

#ifndef EPOLL_TRY_VISITPOSTHANDLER_H
#define EPOLL_TRY_VISITPOSTHANDLER_H

#include "../Buffer.h"
#include "../Util.h"
#include "../storage.h"

namespace VisitPostHandler {
    void process(Buffer *buffer) {
        char *buf = buffer->rdBuf;
        if (buf[13] == 110) { //neW
            char *startPos = ::Util::getStartBodyIdx(buf + 37); // +24
            if (startPos - buf >= buffer->readCount) {
                buffer->writeBadRequest();
                return;
            }
            int fieldsCnt = 5;
            char *location = 0;
            char *user = 0;
            char *visitedAt = 0;
            char *mark = 0;
            char *idStr = 0;
            char *tmp;
            startPos = Util::jsonFirstStringDelim(startPos);
            while (fieldsCnt != 0) {
                switch (*startPos) {
                    case 'l'://location
                        startPos += 11;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        location = startPos;
                        startPos = tmp;
                        break;
                    case 'u'://user
                        startPos += 7;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        user = startPos;
                        startPos = tmp;
                        break;
                    case 'v'://visited_at
                        startPos += 13;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        visitedAt = startPos;
                        startPos = tmp;
                        break;
                    case 'm'://mark
                        startPos += 7;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        mark = startPos;
                        startPos = tmp;
                        break;
                    default://email
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
            Visit *visit = ::storage::visits + id;
            visit->id = id;
            visit->location = Util::parseUInt(location);
            visit->user = Util::parseUInt(user);
            visit->visitedAt = Util::parseUInt(visitedAt);
            visit->mark = Util::parseUInt(mark);
            ::storage::users[visit->user].visits.push_back(visit);
            std::sort(::storage::users[visit->user].visits.begin(), ::storage::users[visit->user].visits.end(), [](const Visit *first, const Visit *second) {
                return first->visitedAt < second->visitedAt;
            });
            ::storage::locations[visit->location].visits.push_back(visit);
            visit->updateVisitOutput(buffer->rdBuf, buffer->smallBuf);
        } else {
            int visitId = Util::tryParsePositiveIntPostPath(buf + 13); // 11 pos
            if (visitId == INT_MIN) {
                buffer->writeNotFound();
                return;
            }

            Visit *visit = visitId < storage::visitsArrayLength ? storage::visits + visitId : NULL;
            if (visit == NULL || visit->id == 0) {
                buffer->writeNotFound();
                return;
            }
            char *startPos = ::Util::getStartBodyIdx(buf + 35); // +22
            if (startPos - buf >= buffer->readCount) {
                buffer->writeBadRequest();
                return;
            }
            char *location = 0;
            char *user = 0;
            char *visitedAt = 0;
            char *mark = 0;
            char *tmp;
            startPos = Util::jsonFirstStringDelim(startPos);
            while (startPos - buf < buffer->readCount) {
                switch (*startPos) {
                    case 'l'://location
                        startPos += 11;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        location = startPos;
                        startPos = tmp;
                        break;
                    case 'u'://user
                        startPos += 7;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        user = startPos;
                        startPos = tmp;
                        break;
                    case 'v'://visited_at
                        startPos += 13;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        visitedAt = startPos;
                        startPos = tmp;
                        break;
                    case 'm'://mark
                        startPos += 7;
                        tmp = Util::advanceIfUInt(startPos);
                        if (tmp == 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        mark = startPos;
                        startPos = tmp;
                }
            }
            buffer->writePostOk();
            if (location != 0) {
                std::vector<Visit *> &vec = ::storage::locations[visit->location].visits;
                for (std::vector<Visit *>::iterator it = vec.begin(); it != vec.end(); ++it) {
                    if (*it == visit) {
                        *it = *vec.rbegin();
                        vec.pop_back();
                        break;
                    }
                }
                visit->location = Util::parseUInt(location);//replace location
                ::storage::locations[visit->location].visits.push_back(visit);
            }
            if (user != 0) {
                std::vector<Visit *> &vec = ::storage::users[visit->user].visits;
                for (std::vector<Visit *>::iterator it = vec.begin(); it != vec.end(); ++it) {
                    if (*it == visit) {
                        *it = *vec.rbegin();
                        vec.pop_back();
                        break;
                    }
                }
                std::sort(::storage::users[visit->user].visits.begin(), ::storage::users[visit->user].visits.end(), [](const Visit *first, const Visit *second) {
                    return first->visitedAt < second->visitedAt;
                });
                visit->user = Util::parseUInt(user);//replace user
                ::storage::users[visit->user].visits.push_back(visit);
                std::sort(::storage::users[visit->user].visits.begin(), ::storage::users[visit->user].visits.end(), [](const Visit *first, const Visit *second) {
                    return first->visitedAt < second->visitedAt;
                });
            }
            if (visitedAt != 0) {
                visit->visitedAt = Util::parseUInt(visitedAt);
                std::sort(::storage::users[visit->user].visits.begin(), ::storage::users[visit->user].visits.end(), [](const Visit *first, const Visit *second) {
                    return first->visitedAt < second->visitedAt;
                });
            }
            if (mark != 0) {
                visit->mark = Util::parseUInt(mark);
            }
            visit->updateVisitOutput(buffer->rdBuf, buffer->smallBuf);
        }
    }
};


#endif //EPOLL_TRY_VISITPOSTHANDLER_H
