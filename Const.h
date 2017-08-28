//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_CONSTRESPONSE_H
#define EPOLL_TRY_CONSTRESPONSE_H

#include <cstring>

namespace Const {
    static constexpr const char *NOT_FOUND = "HTTP/1.1 404 Not Found\r\nServer: d\r\nContent-Length: 0\r\nContent-Type: application/json\r\n\r\n";
    static constexpr const char *BAD_REQUEST = "HTTP/1.1 400 Bad Request\r\nServer: d\r\nContent-Length: 0\r\nContent-Type: application/json\r\n\r\n";
    static constexpr const char *POST_OK = "HTTP/1.1 200 OK\r\nServer: d\r\nContent-Length: 2\r\nContent-Type: application/json\r\n\r\n{}";
    const char OK_PREPARED[] = "HTTP/1.1 200 OK\r\nServer: d\r\nContent-Type: application/json\r\nContent-Length: ";
    const char OK_PREPARED_SECOND[] = "\r\n\r\n";
    const char AVG_FORMAT[] = "HTTP/1.1 200 OK\r\nServer: d\r\nContent-Type: application/json\r\nContent-Length: 16\r\n\r\n{\"avg\": -.-----}";

    extern long long currentTimestamp;
    extern int currentStage;

    const size_t OK_PREPARED_SZ = sizeof(OK_PREPARED) / sizeof(char) - 1;
    const size_t NOT_FOUND_SZ = 101;
    const size_t BAD_REQUEST_SZ = 103;
    const size_t POST_OK_SZ = 96;

    const char LOCATION_FORMAT_ID[] = "{\"id\":";
    const char LOCATION_FORMAT_PLACE[] = ",\"place\":\"";
    const char LOCATION_FORMAT_COUNTRY[] = "\",\"country\":\"";
    const char LOCATION_FORMAT_CITY[] = "\",\"city\":\"";
    const char LOCATION_FORMAT_DISTANCE[] = "\",\"distance\":";

    const char USER_FORMAT_ID[] = "{\"id\":";
    const char USER_FORMAT_EMAIL[] = ",\"email\":\"";
    const char USER_FORMAT_FIRST_NAME[] = "\",\"first_name\":\"";
    const char USER_FORMAT_LAST_NAME[] = "\",\"last_name\":\"";
    const char USER_FORMAT_GENDER_AND_BD[] = "\",\"gender\":\"-\",\"birth_date\":"; // add '}' to the end
    const int USER_FORMAT_GENDER_AND_BD_SZ = sizeof(USER_FORMAT_GENDER_AND_BD) / sizeof(char);

    const char VISIT_FORMAT_ID[] = "{\"id\":";
    const char VISIT_FORMAT_LOCATION[] = ",\"location\":";
    const char VISIT_FORMAT_USER[] = ",\"user\":";
    const char VISIT_FORMAT_VISITED_AT[] = ",\"visited_at\":";
    const char VISIT_FORMAT_MARK_AND_STOP[] = ",\"mark\":-}";
};


#endif //EPOLL_TRY_CONSTRESPONSE_H
