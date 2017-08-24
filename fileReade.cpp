//
// Created by dragoon on 8/20/17.
//

#include "fileReade.h"

size_t::fileReade::maxLengthResponseUser;
size_t::fileReade::maxLengthResponseLocation;
size_t::fileReade::maxLengthResponseVisit;

void ::fileReade::readData(const char *path) {
    DIR *dir;
    struct dirent *ent;
    std::string dirPath = path;
    if ((dir = opendir(path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (!strcmp(ent->d_name + strlen(ent->d_name) - 5, ".json")) {
//                fprintf(stdout, "%s\n", ent->d_name);
                std::ifstream myfile(dirPath + '/' + ent->d_name);
                std::string str;
                std::getline(myfile, str);
                myfile.close();
                std::stringstream lineStream(str);
                std::string token;
                lineStream >> token;
                switch (token[2]) {
                    case 'l':
                        readLocations(&lineStream);
                        break;
                    case 'u':
                        readUsers(&lineStream);
                        break;
                    default:
                        readVisits(&lineStream);
                        break;
                }
            } else if (!strcmp(ent->d_name, "options.txt")) {
                std::ifstream myfile(dirPath + '/' + ent->d_name);
                std::string str;
                std::getline(myfile, str);
                ::Const::currentTimestamp = ::Util::tryParsePositiveInt(str.c_str());
                std::getline(myfile, str);
                ::Const::currentStage = ::Util::tryParsePositiveInt(str.c_str());
                myfile.close();
            }
        }
        closedir(dir);
        storage::printCnt();
    } else {
        fprintf(stdout, "can't open dir\n");
    }
    std::cout << "timestamp " << ::Const::currentTimestamp << std::endl;
    std::cout << "stage " << ::Const::currentStage << std::endl;
    std::cout << "max user length " << maxLengthResponseUser << std::endl;
    std::cout << "max location length " << maxLengthResponseLocation << std::endl;
    std::cout << "max visit length " << maxLengthResponseVisit << std::endl;
}

void ::fileReade::readLocations(std::stringstream *tokenizer) {
    Location tmpLocation;
    std::string token;
    do {
        (*tokenizer) >> token;
        tmpLocation.distance = nextInt(tokenizer);
        (*tokenizer) >> token;
        nextString(tokenizer, tmpLocation.city);
        (*tokenizer) >> token;
        nextString(tokenizer, &tmpLocation.place);
        (*tokenizer) >> token;
        tmpLocation.id = nextInt(tokenizer);
        (*tokenizer) >> token;
        nextString(tokenizer, tmpLocation.country);
        storage::locations[tmpLocation.id] = tmpLocation;

        storage::locations[tmpLocation.id].updateLocationOutput(&Buffer::instance);
        ::fileReade::maxLengthResponseLocation = std::max(::fileReade::maxLengthResponseLocation, storage::locations[tmpLocation.id].getSize);
    } while (tokenizer->rdbuf()->in_avail());
}

void ::fileReade::readUsers(std::stringstream *tokenizer) {
    User tmpUser;
    std::string token;
    do {
        (*tokenizer) >> token;
        nextString(tokenizer, tmpUser.firstName);
        (*tokenizer) >> token;
        nextString(tokenizer, tmpUser.lastName);
        (*tokenizer) >> token;
        tmpUser.birthDate = nextInt(tokenizer);
        (*tokenizer) >> token;
        (*tokenizer) >> token;
        tmpUser.male = token[1];
        (*tokenizer) >> token;
        tmpUser.id = nextInt(tokenizer);
        (*tokenizer) >> token;
        nextString(tokenizer, tmpUser.email);
        storage::users[tmpUser.id] = tmpUser;

        storage::users[tmpUser.id].updateUserOutput(&Buffer::instance);
        ::fileReade::maxLengthResponseUser = std::max(::fileReade::maxLengthResponseUser, storage::users[tmpUser.id].getSize);
    } while (tokenizer->rdbuf()->in_avail());
}

void ::fileReade::readVisits(std::stringstream *tokenizer) {
    Visit tmpVisit;
    std::string token;
    int cnt = 0;
    do {
        ++cnt;
        (*tokenizer) >> token;
        tmpVisit.user = nextInt(tokenizer);
        (*tokenizer) >> token;
        tmpVisit.location = nextInt(tokenizer);
        (*tokenizer) >> token;
        tmpVisit.visitedAt = nextInt(tokenizer);
        (*tokenizer) >> token;
        tmpVisit.id = nextInt(tokenizer);
        (*tokenizer) >> token;
        tmpVisit.mark = nextMark(tokenizer);
        storage::visits[tmpVisit.id] = tmpVisit;
        storage::locations[tmpVisit.location].visits.push_back(storage::visits + tmpVisit.id);
        storage::users[tmpVisit.user].visits.push_back(storage::visits + tmpVisit.id);

        storage::visits[tmpVisit.id].updateVisitOutput(&Buffer::instance);
        ::fileReade::maxLengthResponseVisit = std::max(::fileReade::maxLengthResponseVisit, storage::visits[tmpVisit.id].getSize);
    } while (tokenizer->rdbuf()->in_avail());
}

void ::fileReade::nextString(std::stringstream *tokenizer, char *buf) {
    std::string string;
    *tokenizer >> string;
    int idx = 0;
    int stringIdx = 1;
    while (true) {
        while (stringIdx != string.length()) {
            char ch = string[stringIdx++];
            if (ch == '"') {
                buf[idx] = 0;
                return;
            }
            buf[idx++] = ch;
        }
        stringIdx = 0;
        buf[idx++] = ' ';
        *tokenizer >> string;
    }
}

void ::fileReade::nextString(std::stringstream *tokenizer, std::string *buf) {
    std::string string;
    *tokenizer >> string;
    *buf = "";
    int stringIdx = 1;
    while (true) {
        while (stringIdx != string.length()) {
            char ch = string[stringIdx++];
            if (ch == '"') {
                return;
            }
            *buf += ch;
        }
        stringIdx = 0;
        *buf += ' ';
        *tokenizer >> string;
    }
}

int ::fileReade::nextInt(std::stringstream *tokenizer) {
    std::string string;
    *tokenizer >> string;
    int val = 0;
    int idx = 0;
    bool neg = false;
    if (string[0] == '-') {
        idx = 1;
        neg = true;
    }

    do {
        val *= 10;
        val += string[idx] - '0';
    } while (string[++idx] != ',');
    return neg ? -val : val;
}

int ::fileReade::nextMark(std::stringstream *tokenizer) {
    std::string string;
    *tokenizer >> string;
    int val = 0;
    int idx = 0;
    bool neg = false;
    if (string[0] == '-') {
        idx = 1;
        neg = true;
    }
    do {
        val *= 10;
        val += string[idx] - '0';
    } while (string[++idx] != '}');
    return neg ? -val : val;
}