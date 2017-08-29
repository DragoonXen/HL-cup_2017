//
// Created by dragoon on 8/20/17.
//

#include <algorithm>
#include "storage.h"

User *::storage::users;
Location *::storage::locations;
Visit *::storage::visits;

void ::storage::printCnt() {
    std::vector<int> visitsCount;
    int sum = 0;
    size_t maxVal = 0;
    int cnt = 0;
    int size = locationsArrayLength;
    for (int i = 0; i != size; ++i) {
        if (locations[i].id == 0) {
            continue;
        }
        ++cnt;
        visitsCount.push_back(locations[i].visits.size());
        sum += locations[i].visits.size();
        maxVal = std::max(maxVal, locations[i].visits.size());
    }
    fprintf(stdout, "%d locations\n", cnt);
    fprintf(stdout, "%zu max visits per location\n", maxVal);
    fprintf(stdout, "%.5f avg visits per location\n", ((float) sum) / visitsCount.size());
    std::sort(visitsCount.begin(), visitsCount.end());
    fprintf(stdout, "%d  median visits per location\n", visitsCount[visitsCount.size() / 2]);
    sum = 0;
    maxVal = 0;
    cnt = 0;
    visitsCount.clear();
    size = usersArrayLength;
    for (int i = 0; i != size; ++i) {
        if (users[i].id == 0) {
            continue;
        }
        ++cnt;
        visitsCount.push_back(users[i].visits.size());
        std::sort(users[i].visits.begin(), users[i].visits.end(), [](const Visit *first, const Visit *second) {
            return first->visitedAt < second->visitedAt;
        });
        sum += users[i].visits.size();
        maxVal = std::max(maxVal, users[i].visits.size());
    }
    fprintf(stdout, "%d users\n", cnt);
    fprintf(stdout, "%zu max visits per user\n", maxVal);
    fprintf(stdout, "%.5f avg visits per user\n", ((float) sum) / visitsCount.size());
    std::sort(visitsCount.begin(), visitsCount.end());
    fprintf(stdout, "%d  median visits per user\n", visitsCount[visitsCount.size() / 2]);

    cnt = 0;
    size = visitsArrayLength;
    for (int i = 0; i != size; ++i) {
        if (visits[i].id == 0) {
            continue;
        }
        ++cnt;
    }
    fprintf(stdout, "%d visits\n", cnt);
}
