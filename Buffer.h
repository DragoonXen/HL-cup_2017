//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_BUFFER_H
#define EPOLL_TRY_BUFFER_H

#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/uio.h>
#include "Const.h"

class Buffer {
public:
    static constexpr size_t AVG_FORMAT_SZ = 111;
private:
    const char *bufToWrite;
    iovec ovec[2];
    bool multiWriteType;
public:
    ssize_t readCount = 0;
    char avgFormat[AVG_FORMAT_SZ + 1]; // zero char
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
    }

    inline void writeBadRequest() {
        this->bufToWrite = Const::BAD_REQUEST;
        this->writeLength = Const::BAD_REQUEST_SZ;
    }

    inline void writePostOk() {
        this->bufToWrite = Const::POST_OK;
        this->writeLength = Const::POST_OK_SZ;
    }

    inline void writeResponse(char *buf, size_t size, char *buf2, size_t size2) {
        multiWriteType = true;
        ovec[0].iov_base = buf;
        ovec[0].iov_len = size;
        ovec[1].iov_base = buf2;
        ovec[1].iov_len = size2;
    }

    inline void writeResponse(char *buf, size_t size) {
        this->bufToWrite = buf;
        this->writeLength = size;
    }

    inline void readSource() {
        readCount = read(source, rdBuf, sizeof rdBuf);
        writePos = writeLength = 0;
    }

    inline void writeResponse() {
        ssize_t writtenBytes;
        if (multiWriteType) {
            writtenBytes = writev(source, ovec, 2);
            writeLength = ovec[0].iov_len + ovec[1].iov_len;
            multiWriteType = false;
        } else {
            writtenBytes = write(source, bufToWrite + writePos, writeLength - writePos);
        }
        if (writtenBytes < 0) {
            std::cout << "error while write, -1 received" << std::endl;
        }
        writePos += writtenBytes;
        if (writePos != writeLength) {
            std::cout << "not fully writen " << writePos << ' ' << writeLength << std::endl;
            return;
        } else {
            if (closeConnection) {
                close(source);
            }
            return;
        }
    }

    inline bool complete() {
        return writePos == writeLength;
    }
};

#endif //EPOLL_TRY_BUFFER_H
