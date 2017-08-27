//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_USERGETHANDLER_H
#define EPOLL_TRY_USERGETHANDLER_H


#include <algorithm>
#include "../Buffer.h"
#include "../model/Visit.h"
#include "../model/User.h"
#include "../storage.h"

namespace UserGetHandler {

    char VISIT_FORMAT_PLACE[] = "{\"place\":\"";
    char VISITED_AT[] = "\",\"visited_at\":";
    char MARK[] = ",\"mark\": }";
    char USER_VISITS_FORMAT[] = "{\"visits\":[";
    char USER_VISITS_ENDING[] = "]}";
    char NO_VISITS_BUF[] = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nContent-Type: application/json\r\nContent-Length: 13\r\n\r\n{\"visits\":[]}";
    const size_t NO_VISITS_BUF_SZ = sizeof(NO_VISITS_BUF) / sizeof(char) - 1;

    inline char *formatVisit(char *buffer, const Visit *visit, char *smallBuf) {
        buffer += ::Util::copyCharArray(VISIT_FORMAT_PLACE, buffer);
        buffer += ::Util::copyCharArray(::storage::locations[visit->location].place, buffer);

        buffer += ::Util::copyCharArray(VISITED_AT, buffer);
        buffer += ::Util::intToStringBytes(visit->visitedAt, buffer, smallBuf);

        buffer += ::Util::copyCharArray(MARK, buffer);
        *(buffer - 2) = (char) (visit->mark + '0');
        return buffer;
    }

    inline void writeResponse(Buffer *buffer, const std::vector<Visit *> &visits) {
        char *tempAnsBuffer = buffer->rdBuf;
        tempAnsBuffer += ::Util::copyCharArray(USER_VISITS_FORMAT, tempAnsBuffer);
        {
            tempAnsBuffer = formatVisit(tempAnsBuffer, visits[0], buffer->smallBuf);
            for (int i = 1; i != visits.size(); ++i) {
                *tempAnsBuffer++ = ',';
                tempAnsBuffer = formatVisit(tempAnsBuffer, visits[i], buffer->smallBuf);
            };
        }
        tempAnsBuffer += ::Util::copyCharArray(USER_VISITS_ENDING, tempAnsBuffer);
//    *tempAnsBuffer = 0;
//    buffer->writeResponse(buffer->rdBuf, )

        char *writeBuf = buffer->wrBuf + Const::OK_PREPARED_SZ;
//        writeBuf += ::Util::copyCharArray(Const::OK_PREPARED, writeBuf);
        writeBuf += ::Util::uintToStringBytes((int) (tempAnsBuffer - buffer->rdBuf), writeBuf, buffer->smallBuf);
        writeBuf += ::Util::copyCharArray(Const::OK_PREPARED_SECOND, writeBuf);
        buffer->writeResponse(buffer->wrBuf, writeBuf - buffer->wrBuf, buffer->rdBuf, tempAnsBuffer - buffer->rdBuf);
//    *writeBuf = 0;

//    buffer->writeResponse(buffer->wrBuf, writeBuf - buffer->wrBuf);
    }

    void process(Buffer *buffer) {
        char *buf = buffer->rdBuf;
        int userId = Util::tryParsePositiveIntPath(buf + 11); // 11 pos
        if (userId == INT_MIN) {
            buffer->writeNotFound();
            return;
        }
        User *user = userId < storage::usersArrayLength ? storage::users + userId : NULL;
        if (user == NULL || user->id == 0) {
            buffer->writeNotFound();
            return;
        }
        char *lastPathChr = ::Util::getLastPathCharPtr(buf + 12);
        if (*(lastPathChr - 1) != 115) { // just get by id
            buffer->writeResponse(user->getBuffer, user->getSize);
        } else { // user visits
            bool hasQuery = *(lastPathChr++) == '?';
            if (hasQuery) {
                int toDistance = INT_MIN;
                int toDate = INT_MIN;
                int fromDate = INT_MIN;
                char *country = 0;
                int cnt = ::Util::parseQuery(lastPathChr, buffer->pathBuf);
                for (int i = 0; i != cnt; i += 2) {
                    switch (buffer->pathBuf[i][3]) {
                        case 'i'://1766092660
                            toDistance = ::Util::tryParsePositiveInt(buffer->pathBuf[i + 1]);
                            if (toDistance == INT_MIN) {
                                buffer->writeBadRequest();
                                return;
                            }
                            break;
                        case 'a'://1631874932
                            toDate = ::Util::tryParsePositiveInt(buffer->pathBuf[i + 1]);
                            if (toDate == INT_MIN) {
                                buffer->writeBadRequest();
                                return;
                            }
                            break;
                        case 'm'://1836020326
                            fromDate = ::Util::tryParsePositiveInt(buffer->pathBuf[i + 1]);
                            if (fromDate == INT_MIN) {
                                buffer->writeBadRequest();
                                return;
                            }
                            break;
                        default: //1853189987
                            country = buffer->pathBuf[i + 1];
                    }
                }
                if (user->visits.empty()) {
                    buffer->writeResponse(NO_VISITS_BUF, NO_VISITS_BUF_SZ);
                    return;
                }
                std::vector<Visit *> visits(user->visits.begin(), user->visits.end());
                if (country != 0) {
                    int hash = ::Util::calcHashUrl(country);
                    for (std::vector<Visit *>::iterator it = visits.begin(); it != visits.end();) {
                        if (storage::locations[(*it)->location].countryHash != hash) {
                            *it = *visits.rbegin();
                            visits.pop_back();
                        } else {
                            ++it;
                        }
                    }
                    if (visits.empty()) {
                        buffer->writeResponse(NO_VISITS_BUF, NO_VISITS_BUF_SZ);
                        return;
                    }
                }
                if (toDistance != INT_MIN) {
                    for (std::vector<Visit *>::iterator it = visits.begin(); it != visits.end();) {
                        if (storage::locations[(*it)->location].distance >= toDistance) {
                            *it = *visits.rbegin();
                            visits.pop_back();
                        } else {
                            ++it;
                        }
                    }
                    if (visits.empty()) {
                        buffer->writeResponse(NO_VISITS_BUF, NO_VISITS_BUF_SZ);
                        return;
                    }
                }
                if (toDate != INT_MIN) {
                    for (std::vector<Visit *>::iterator it = visits.begin(); it != visits.end();) {
                        if ((*it)->visitedAt >= toDate) {
                            *it = *visits.rbegin();
                            visits.pop_back();
                        } else {
                            ++it;
                        }
                    }
                    if (visits.empty()) {
                        buffer->writeResponse(NO_VISITS_BUF, NO_VISITS_BUF_SZ);
                        return;
                    }
                }
                if (fromDate != INT_MIN) {
                    for (std::vector<Visit *>::iterator it = visits.begin(); it != visits.end();) {
                        if ((*it)->visitedAt <= fromDate) {
                            *it = *visits.rbegin();
                            visits.pop_back();
                        } else {
                            ++it;
                        }
                    }
                    if (visits.empty()) {
                        buffer->writeResponse(NO_VISITS_BUF, NO_VISITS_BUF_SZ);
                        return;
                    }
                }
                std::sort(visits.begin(), visits.end(), [](const Visit *first, const Visit *second) {
                    return first->visitedAt < second->visitedAt;
                });
                writeResponse(buffer, visits);
            } else {
                if (user->visits.empty()) {
                    buffer->writeResponse(NO_VISITS_BUF, NO_VISITS_BUF_SZ);
                    return;
                }
                std::sort(user->visits.begin(), user->visits.end(), [](const Visit *first, const Visit *second) {
                    return first->visitedAt < second->visitedAt;
                });
                writeResponse(buffer, user->visits);
            }
        }
    }
};


#endif //EPOLL_TRY_USERGETHANDLER_H
