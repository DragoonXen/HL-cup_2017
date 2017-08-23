//
// Created by dragoon on 8/20/17.
//

#include "Const.h"

char ::Const::NOT_FOUND[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nContent-Type: application/json\r\n\r\n";
char ::Const::BAD_REQUEST[] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nContent-Type: application/json\r\n\r\n";
char ::Const::POST_OK[] = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nContent-Type: application/json\r\n\r\n{}";
char ::Const::OK_PREPARED[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: ";
char ::Const::OK_PREPARED_SECOND[] = "\r\n\r\n";

long long ::Const::currentTimestamp = 0;
int ::Const::currentStage = 0;

size_t::Const::NOT_FOUND_SZ = sizeof(NOT_FOUND) / sizeof(char) - 1;
size_t::Const::BAD_REQUEST_SZ = sizeof(BAD_REQUEST) / sizeof(char) - 1;
size_t::Const::POST_OK_SZ = sizeof(POST_OK) / sizeof(char) - 1;
