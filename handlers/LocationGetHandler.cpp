//
// Created by dragoon on 8/20/17.
//

#include <iostream>
#include "LocationGetHandler.h"

namespace LocationGetHandler {
    char EMPTY_AVG[] = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nContent-Type: application/json\r\nContent-Length: 10\r\n\r\n{\"avg\": 0}";
    const size_t EMPTY_AVG_SZ = sizeof(EMPTY_AVG) / sizeof(char) - 1;
//    char AVG_FORMAT[] = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nContent-Type: application/json\r\nContent-Length: 16\r\n\r\n{\"avg\": -.-----}";
//    const size_t AVG_FORMAT_SZ = sizeof(AVG_FORMAT) / sizeof(char) - 1;

    const long long secShift[] = {0, -31536000, -63158400, -94694400, -126230400, -157766400, -189388800, -220924800, -252460800, -283996800, -315619200,
                                  -347155200, -378691200, -410227200, -441849600, -473385600, -504921600, -536457600, -568080000, -599616000, -631152000,
                                  -662688000, -694310400, -725846400, -757382400, -788918400, -820540800, -852076800, -883616400, -915152400, -946774800,
                                  -978310800, -1009846800, -1041382800, -1073005200, -1104541200, -1136077200, -1167609600, -1199232000, -1230768000,
                                  -1262304000, -1293840000, -1325462400, -1356998400, -1388534400, -1420070400, -1451692800, -1483228800, -1514764800,
                                  -1546300800, -1577923200, -1609459200, -1640995200, -1672531200, -1704153600, -1735689600, -1767225600, -1798761600,
                                  -1830384000, -1861920000, -1893456000, -1924992000, -1956614400, -1988150400, -2019686400, -2051222400, -2082844800,
                                  -2114380800, -2145916800, -2177452800, -2209075200, -2240611200, -2272147200, -2303683200, -2335302000, -2366838000,
                                  -2398374000, -2429913600, -2461536000, -2493072000, -2524608000, -2556144000, -2587766400, -2619302400, -2650838400,
                                  -2682374400, -2713996800, -2745532800, -2777065200, -2808601200, -2840223600, -2871759600, -2903295600, -2934831600,
                                  -2966453400, -2997989400, -3029525400, -3061061400, -3092683800, -3124219800, -3155755800, -3187291800, -3218914200,
                                  -3250450200, -3281986200, -3313522200, -3345144600, -3376680600, -3408216600, -3439752600, -3471375000, -3502911000,
                                  -3534447000, -3565983000, -3597605400, -3629141400, -3660677400, -3692213400, -3723753600, -3755289600, -3786825600,
                                  -3818361600, -3849984000, -3881520000, -3913056000, -3944592000, -3976214400, -4007750400, -4039286400, -4070822400,
                                  -4102444800, -4133980800, -4165516800, -4197052800, -4228675200, -4260211200, -4291747200, -4323283200, -4354905600,
                                  -4386441600, -4417977600, -4449513600, -4481136000, -4512672000, -4544208000, -4575744000, -4607366400, -4638902400,
                                  -4670438400, -4701974400, -4733596800, -4765132800, -4796668800, -4828204800, -4859827200, -4891363200, -4922899200,
                                  -4954435200, -4986057600, -5017593600, -5049129600, -5080665600, -5112288000, -5143824000, -5175360000, -5206896000,
                                  -5238518400, -5270054400, -5301590400, -5333126400, -5364748800, -5396284800, -5427820800, -5459356800, -5490979200,
                                  -5522515200, -5554051200, -5585587200, -5617209600, -5648745600, -5680281600, -5711817600, -5743440000, -5774976000,
                                  -5806512000, -5838048000, -5869670400, -5901206400, -5932742400, -5964278400, -5995900800, -6027436800, -6058972800,
                                  -6090508800, -6122131200, -6153667200, -6185203200, -6216739200, -6248361600, -6279897600};
}

void LocationGetHandler::process(Buffer *buffer) {
    char *buf = buffer->rdBuf;
    int locationId = Util::tryParsePositiveIntPath(buf + 15); // 11 pos
    if (locationId == INT_MIN) {
        buffer->writeNotFound();
        return;
    }

    Location *location = locationId < storage::locationsArrayLength ? storage::locations + locationId : NULL;
    if (location == NULL || location->id == 0) {
        buffer->writeNotFound();
        return;
    }

    char *lastPathChr = ::Util::getLastPathCharPtr(buf + 16);
    if (*(lastPathChr - 1) != 103) {// just get by id
        buffer->writeResponse(location->getBuffer, location->getSize);
    } else {
//        // avg
        bool hasQuery = *lastPathChr++ == '?';
        if (hasQuery) {
//            if (locationId == 191) {
//                ::Util::splitQuery(lastPathChr);
//                std::cout << lastPathChr << std::endl;
//                for (Visit *visit : location->visits) {
//                    User *user = ::storage::users + visit->user;
//                    std::cout << "birth date " << user->birthDate << ", visitTime " << visit->visitedAt << ", mark " << visit->mark << std::endl;
//                }
//            }
            std::vector<char *> query;
            ::Util::parseQuery(lastPathChr, query);
            char male = 0;
            int fromDate = INT_MIN;
            int toDate = INT_MIN;
            int fromAge = INT_MIN;
            int toAge = INT_MIN;
            for (int i = 0; i != query.size(); i += 2) {
                switch (query[i][2]) {
                    case 'n'://gender
                        switch (query[i + 1][0]) {
                            case 'm':
                                male = 'm';
                                break;
                            case 'f':
                                male = 'f';
                                break;
                            default:
                                buffer->writeBadRequest();
                                return;
                        }
                        if (query[i + 1][1] != 0) {
                            buffer->writeBadRequest();
                            return;
                        }
                        break;
                    case 'o': // fromAge and fromDate
                        switch (query[i][4]) {
                            case 'A':// fromAge
                                fromAge = ::Util::tryParsePositiveInt(query[i + 1]);
                                if (fromAge == INT_MIN) {
                                    buffer->writeBadRequest();
                                    return;
                                }
                                break;
                            default:// fromDate
                                fromDate = ::Util::tryParseInt(query[i + 1]);
                                if (fromDate == INT_MIN) {
                                    buffer->writeBadRequest();
                                    return;
                                }
                                break;
                        }
                        break;
                    case 'D'://toDate
                        toDate = ::Util::tryParseInt(query[i + 1]);
                        if (toDate == INT_MIN) {
                            buffer->writeBadRequest();
                            return;
                        }
                        break;
                    default://toAge
                        toAge = ::Util::tryParsePositiveInt(query[i + 1]);
                        if (toAge == INT_MIN) {
                            buffer->writeBadRequest();
                            return;
                        }
                }
            }
//            if (locationId == 191) {
//                std::cout << "male " << male << " fromDate " << fromDate << " toDate " << toDate << " fromAge " << fromAge << " toAge " << toAge << std::endl;
//            }
            if (location->visits.empty()) {
                buffer->writeResponse(EMPTY_AVG, EMPTY_AVG_SZ);
                return;
            }
            std::vector<Visit *> visits(location->visits.begin(), location->visits.end());
            if (male != 0) {
                for (std::vector<Visit *>::iterator it = visits.begin(); it != visits.end();) {
                    if (::storage::users[(*it)->user].male != male) {
                        *it = *visits.rbegin();
                        visits.pop_back();
                    } else {
                        ++it;
                    }
                }
                if (visits.empty()) {
                    buffer->writeResponse(EMPTY_AVG, EMPTY_AVG_SZ);
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
                    buffer->writeResponse(EMPTY_AVG, EMPTY_AVG_SZ);
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
                    buffer->writeResponse(EMPTY_AVG, EMPTY_AVG_SZ);
                    return;
                }
            }
            if (fromAge != INT_MIN) {
                fromAge = (int) (::Const::currentTimestamp + secShift[fromAge]);
                for (std::vector<Visit *>::iterator it = visits.begin(); it != visits.end();) {
                    if (::storage::users[(*it)->user].birthDate >= fromAge) {
                        *it = *visits.rbegin();
                        visits.pop_back();
                    } else {
                        ++it;
                    }
                }
                if (visits.empty()) {
                    buffer->writeResponse(EMPTY_AVG, EMPTY_AVG_SZ);
                    return;
                }
            }
            if (toAge != INT_MIN) {
                toAge = (int) (::Const::currentTimestamp + secShift[toAge]);
                for (std::vector<Visit *>::iterator it = visits.begin(); it != visits.end();) {
                    if (::storage::users[(*it)->user].birthDate <= toAge) {
                        *it = *visits.rbegin();
                        visits.pop_back();
                    } else {
                        ++it;
                    }
                }
                if (visits.empty()) {
                    buffer->writeResponse(EMPTY_AVG, EMPTY_AVG_SZ);
                    return;
                }
            }
            int sum = 0;
            for (Visit *visit : visits) {
                sum += visit->mark;
            }
            // write to buffer
            writeAvgOutput(buffer, (size_t) sum, visits.size());
        } else {
            if (location->visits.empty()) {
                buffer->writeResponse(EMPTY_AVG, EMPTY_AVG_SZ);
                return;
            }
            int sum = 0;
            for (Visit *visit : location->visits) {
                sum += visit->mark;
            }
            writeAvgOutput(buffer, (size_t) sum, location->visits.size());
        }
    }
}

inline void LocationGetHandler::writeAvgOutput(Buffer *buffer, size_t sum, size_t count) {
    char *AVG_FORMAT = buffer->avgFormat;
    int result = (int) (sum * 1000000 / count);
    if (result % 10 > 4) {
        result += 10;
    }
    result /= 10;
    for (int i = 109; i != 104; --i) {
        AVG_FORMAT[i] = (char) (result % 10 + '0');
        result /= 10;
    }
    AVG_FORMAT[103] = (char) (result + '0');
//    Util::copyCharArray(AVG_FORMAT, buffer->wrBuf);
    buffer->writeResponse(AVG_FORMAT,  buffer->AVG_FORMAT_SZ);
}


