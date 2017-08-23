//
// Created by dragoon on 8/23/17.
//

#include "../Buffer.h"

void Visit::updateVisitOutput(Buffer *buffer) {
    char *tempAnsBuffer = buffer->rdBuf;

    tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_ID, tempAnsBuffer);
    tempAnsBuffer += ::Util::uintToStringBytes(this->id, tempAnsBuffer, buffer->smallBuf);

    tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_LOCATION, tempAnsBuffer);
    tempAnsBuffer += ::Util::uintToStringBytes(this->location, tempAnsBuffer, buffer->smallBuf);

    tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_USER, tempAnsBuffer);
    tempAnsBuffer += ::Util::uintToStringBytes(this->user, tempAnsBuffer, buffer->smallBuf);

    tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_VISITED_AT, tempAnsBuffer);
    tempAnsBuffer += ::Util::intToStringBytes(this->visitedAt, tempAnsBuffer, buffer->smallBuf);

    tempAnsBuffer += ::Util::copyCharArray(Const::VISIT_FORMAT_MARK_AND_STOP, tempAnsBuffer);
    *(tempAnsBuffer - 2) = (char) (this->mark + '0');
    *tempAnsBuffer = 0;

    char *writeBuf = this->getBuffer;
    writeBuf += ::Util::copyCharArray(Const::OK_PREPARED, writeBuf);
    writeBuf += ::Util::uintToStringBytes((int) (tempAnsBuffer - buffer->rdBuf), writeBuf, buffer->smallBuf);
    writeBuf += ::Util::copyCharArray(Const::OK_PREPARED_SECOND, writeBuf);
    writeBuf += ::Util::copyCharArray(buffer->rdBuf, writeBuf);
//    *writeBuf = 0;
    this->getSize = writeBuf - this->getBuffer;
}
