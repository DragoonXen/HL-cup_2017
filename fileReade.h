//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_FILEREADER_H
#define EPOLL_TRY_FILEREADER_H

#include <sstream>
#include <dirent.h>
#include <cstdio>
#include <cstring>
#include "model/User.h"
#include <iostream>
#include <fstream>
#include "storage.h"

namespace fileReade {

    extern size_t maxLengthResponseUser;
    extern size_t maxLengthResponseLocation;
    extern size_t maxLengthResponseVisit;

    void readData(const char path[]);

    void readLocations(std::stringstream *tokenizer);

    void readUsers(std::stringstream *tokenizer);

    void readVisits(std::stringstream *tokenizer);

    void nextString(std::stringstream *tokenizer, char *buf);

    void nextString(std::stringstream *tokenizer, std::string *buf);

    int nextInt(std::stringstream *tokenizer);

    int nextMark(std::stringstream *tokenizer);
};


#endif //EPOLL_TRY_FILEREADER_H
