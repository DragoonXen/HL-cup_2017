//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_CONSTRESPONSE_H
#define EPOLL_TRY_CONSTRESPONSE_H

#include <cstring>

namespace Const {
    extern char NOT_FOUND[];
    extern char BAD_REQUEST[];
    extern char POST_OK[];
    extern char OK_PREPARED[];
    extern char OK_PREPARED_SECOND[];
    extern char AVG_FORMAT[];

    extern long long currentTimestamp;
    extern int currentStage;

    extern size_t NOT_FOUND_SZ;
    extern size_t BAD_REQUEST_SZ;
    extern size_t POST_OK_SZ;

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
