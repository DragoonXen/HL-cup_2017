//
// Created by dragoon on 8/21/17.
//

#include "LocationPostHandler.h"

void ::LocationPostHandler::process(Buffer *buffer) {
    char *buf = buffer->rdBuf;
    if (buf[16] == 110) {
        char *startPos = ::Util::getStartBodyIdx(buf + 40); // +24
        if (startPos - buf >= buffer->readCount) {
            buffer->writeBadRequest();
            return;
        }
        int fieldsCnt = 5;
        char *city = 0;
        char *place = 0;
        char *country = 0;
        char *distance = 0;
        char *idStr = 0;
        char *tmp;
        //"\"id\": 838,
        // \"distance\": 34,
        // \"place\": \"\\u041f\\u0435\\u0440\\u0435\\u0443\\u043b\\u043e\\u043a\",
        // \"city\": \"\\u0420\\u043e\\u0442\\u0442\\u0435\\u0440\\u0433\\u0430\\u043c\\u0430\",
        // \"country\": \"\\u0411\\u0440\\u0443\\u043d\\u0435\\u0439\"}\r\n\"}\r\nouh@yandex.ru\"}\r\n"
        startPos = Util::jsonFirstStringDelim(startPos);
        while (fieldsCnt != 0) {
            switch (*startPos) {
                case 'c':
                    switch (*(++startPos)) {
                        case 'i':
                            startPos += 6; //city: "
                            tmp = Util::advanceIfString(startPos);
                            if (tmp == 0) {
                                buffer->writeBadRequest();
                                return;
                            }
                            city = startPos + 1;
                            startPos = tmp;
                            break;
                        default:
                            startPos += 9; //country: "
                            tmp = Util::advanceIfString(startPos);
                            if (tmp == 0) {
                                buffer->writeBadRequest();
                                return;
                            }
                            country = startPos + 1;
                            startPos = tmp;
                            break;
                    }
                    break;
                case 'p':
                    startPos += 8; //place: "
                    tmp = Util::advanceIfString(startPos);
                    if (tmp == 0) {
                        buffer->writeBadRequest();
                        return;
                    }
                    place = startPos + 1;
                    startPos = tmp;
                    break;
                case 'd':
                    startPos += 11; //distance": "
                    tmp = Util::advanceIfUInt(startPos);
                    if (tmp == 0) {
                        buffer->writeBadRequest();
                        return;
                    }
                    distance = startPos;
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
        Location *location = ::storage::locations + id;
        location->distance = Util::parseUInt(distance);
        location->id = id;
        Util::unicodeDecode(place, place);
        location->place = place;
        Util::unicodeDecode(city, location->city);
        Util::unicodeDecode(country, location->country);
        location->updateLocationOutput(buffer);
    } else {
        int locationId = Util::tryParsePositiveIntPostPath(buf + 16); // 11 pos
        if (locationId == INT_MIN) {
            buffer->writeNotFound();
            return;
        }

        Location *location = locationId < storage::locationsArrayLength ? storage::locations + locationId : NULL;
        if (location == NULL || location->id == 0) {
            buffer->writeNotFound();
            return;
        }

        char *startPos = Util::getStartBodyIdx(buf + 38); // +22
        if (startPos - buf >= buffer->readCount) {
            buffer->writeBadRequest();
            return;
        }
        char *city = 0;
        char *place = 0;
        char *country = 0;
        char *distance = 0;
        char *tmp;
        startPos = Util::jsonFirstStringDelim(startPos);
        while (startPos - buf < buffer->readCount) {
            switch (*startPos) {
                case 'c':
                    switch (*(++startPos)) {
                        case 'i':
                            startPos += 6; //city: "
                            tmp = Util::advanceIfString(startPos);
                            if (tmp == 0) {
                                buffer->writeBadRequest();
                                return;
                            }
                            city = startPos + 1;
                            startPos = tmp;
                            break;
                        default:
                            startPos += 9; //country: "
                            tmp = Util::advanceIfString(startPos);
                            if (tmp == 0) {
                                buffer->writeBadRequest();
                                return;
                            }
                            country = startPos + 1;
                            startPos = tmp;
                            break;
                    }
                    break;
                case 'p':
                    startPos += 8; //place: "
                    tmp = Util::advanceIfString(startPos);
                    if (tmp == 0) {
                        buffer->writeBadRequest();
                        return;
                    }
                    place = startPos + 1;
                    startPos = tmp;
                    break;
                default:
                    startPos += 11; //distance": "
                    tmp = Util::advanceIfUInt(startPos);
                    if (tmp == 0) {
                        buffer->writeBadRequest();
                        return;
                    }
                    distance = startPos;
                    startPos = tmp;
            }
        }
        buffer->writePostOk();
        if (distance != 0) {
            location->distance = Util::parseUInt(distance);
        }
        if (place != 0) {
            Util::unicodeDecode(place, place);
            location->place = place;
        }
        if (city != 0) {
            Util::unicodeDecode(city, location->city);
        }
        if (country != 0) {
            Util::unicodeDecode(country, location->country);
        }
        location->updateLocationOutput(buffer);
    }
}
