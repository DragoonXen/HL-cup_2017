//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_USER_H
#define EPOLL_TRY_USER_H


#include<vector>
#include "Visit.h"
#include "../Util.h"
#include "../Buffer.h"

class Buffer;

class User {
    static const int MAX_BUF_SIZE = 575;
public:
    int id;
    char email[201]; // 100 symb
    char firstName[101]; // 50 chars
    char lastName[101];// 50 chars
    char male;
    int birthDate;
    std::vector<Visit *> visits;

    char getBuffer[MAX_BUF_SIZE];
    size_t getSize;

    void updateUserOutput(Buffer *buffer);

    User &operator=(const User &user);
};

#endif //EPOLL_TRY_USER_H
