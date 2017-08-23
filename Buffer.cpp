////
//// Created by dragoon on 8/20/17.
////
//
#include "Buffer.h"

void Buffer::processRequest() {
    readCount = read(source, rdBuf, sizeof rdBuf);
    writePos = writeLength = 0;
    Routing::process(this);
}
