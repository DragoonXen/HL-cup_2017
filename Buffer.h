//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_BUFFER_H
#define EPOLL_TRY_BUFFER_H

#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include "Routing.h"
#include <iostream>

class Buffer {
public:
    static Buffer instance[THREADS_COUNT];
private:
    char *bufToWrite;
public:
    ssize_t readCount = 0;
    char rdBuf[16 * 1024];
    size_t writeLength = 0;
    size_t writePos = 0;
    char wrBuf[16 * 1024];
    char smallBuf[64];
    int source = 0;
    bool closeConnection;

    inline void writeNotFound() {
        this->bufToWrite = Const::NOT_FOUND;
        this->writeLength = Const::NOT_FOUND_SZ;
        writeResponse();
    }

    inline void writeBadRequest() {
        this->bufToWrite = Const::BAD_REQUEST;
        this->writeLength = Const::BAD_REQUEST_SZ;
        writeResponse();
    }

    inline void writePostOk() {
        this->bufToWrite = Const::POST_OK;
        this->writeLength = Const::POST_OK_SZ;
        writeResponse();
    }

    inline void writeResponse(char *buf, size_t size) {
        this->bufToWrite = buf;
        this->writeLength = size;
        writeResponse();
    }

    inline void processRequest() {
        readCount = read(source, rdBuf, sizeof rdBuf);
        writePos = writeLength = 0;
    }

    inline bool writeResponse() {
        int writtenBytes = write(source, bufToWrite + writePos, writeLength - writePos);
        if (writtenBytes < 0) {
            std::cout << "error while write, -1 received" << std::endl;
        }
        writePos += writtenBytes;
        if (writePos != writeLength) {
            std::cout << "not fully writen " << writePos << writeLength << std::endl;
            return false;
        } else {
            if (closeConnection) {
                close(source);
            }
            return true;
        }
    }

    inline bool complete() {
        return writePos == writeLength;
    }
};

#endif //EPOLL_TRY_BUFFER_H
