//
// Created by dragoon on 8/20/17.
//

#include "VisitGetHandler.h"

namespace VisitGetHandler {

    void process(Buffer *buffer) {
        char *buf = buffer->rdBuf;
        int visitId = Util::tryParsePositiveIntPath(buf + 12); // 11 pos
        if (visitId == INT_MIN) {
            buffer->writeNotFound();
            return;
        }

        Visit *visit = visitId < storage::visitsArrayLength ? storage::visits + visitId : NULL;
        if (visit == NULL || visit->id == 0) {
            buffer->writeNotFound();
            return;
        }

        buffer->writeResponse(visit->getBuffer, visit->getSize);
    }
}