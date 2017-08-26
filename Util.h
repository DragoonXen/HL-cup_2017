//
// Created by dragoon on 8/20/17.
//

#ifndef EPOLL_TRY_UTIL_H
#define EPOLL_TRY_UTIL_H

#include <climits>
#include <vector>
#include <string>
#include <codecvt>

namespace Util {

    inline int copyCharArray(const char *source, char *dest) {
        int i = 0;
        for (; source[i] != 0; ++i) {
            dest[i] = source[i];
        }
        return i;
    }

    inline int uintToStringBytes(int val, char *dest, char *smallBuf) {
        char *destPos = dest;
        char *idx = smallBuf;
        do {
            *idx = (char) (val % 10 + '0');
            ++idx;
            val /= 10;
        } while (val != 0);
        while (idx > smallBuf) {
            *(destPos++) = *(--idx);
        }
        return (int) (destPos - dest);
    }

    inline int intToStringBytes(int val, char *dest, char *smallBuf) {
        char *destPos = dest;
        if (val < 0) {
            val = -val;
            *(destPos++) = '-';
        }
        char *idx = smallBuf;
        do {
            *idx = (char) (val % 10 + '0');
            ++idx;
            val /= 10;
        } while (val != 0);
        while (idx > smallBuf) {
            *(destPos++) = *(--idx);
        }
        return (int) (destPos - dest);
    }

    inline int tryParsePositiveIntPath(char *buf) {
        int val = 0;
        do {
            int newVal = *buf - '0';
            if (newVal < 0 || newVal > 9) {
                return INT_MIN;
            }
            val = val * 10 + newVal;
        } while (*(++buf) > 47);
        return val;
    }

    inline int tryParsePositiveIntPostPath(char *buf) {
        int val = 0;
        do {
            int newVal = *buf - '0';
            if (newVal < 0 || newVal > 9) {
                return INT_MIN;
            }
            val = val * 10 + newVal;
        } while (*(++buf) != '?');
        return val;
    }

    inline char *getLastPathCharPtr(char *buf) {
        while (*buf != ' ' && *buf != '?') {
            ++buf;
        }
        return buf;
    }

    inline void parseQuery(char *buf, std::vector<char *> &vc) {
        vc.push_back(buf);
        do {
            ++buf;
            if (*buf == '&' || *buf == '=') {
                *buf = 0;
                vc.push_back(buf + 1);
            }
        } while (*buf != ' ');
        *buf = 0;
    }

//    inline void splitQuery(char *buf) {
//        do {
//            ++buf;
//        } while (*buf != ' ');
//        *buf = 0;
//    }

    inline char *jsonFirstStringDelim(char *buf) {
        do {
            ++buf;
        } while (*buf != '"');
        return buf + 1;
    }

    inline char *advanceIfUInt(char *buf) {
        while (*buf >= '0' && *buf <= '9') {
            ++buf;
        }
        if (*buf != '}' && *buf != ',') {
            return 0;
        }
        *buf = 0;
        return buf + 3;
    }

    inline char *advanceIfInt(char *buf) {
        if (*buf == '-') {
            ++buf;
        }
        while (*buf >= '0' && *buf <= '9') {
            ++buf;
        }
        if (*buf != '}' && *buf != ',') {
            return 0;
        }
        *buf = 0;
        return buf + 3;
    }

    inline char *advanceIfString(char *buf) {
        if (*buf != '"') {
            return 0;
        }
        buf += 2;
        while (*buf != '"') {
            ++buf;
        }
        *buf = 0;
        return buf + 4;
    }

    inline char *getStartBodyIdx(char *buf) {
        while (*buf != '\n') {
            ++buf;
        }
        buf += 64;
        while (*buf != '\n' || *(buf + 2) != '\n') { // after last header
            ++buf;
        }
        return buf + 3;
    }

    inline int tryParsePositiveInt(const char *integer) {
        int val = 0;

        do {
            int newVal = *integer - '0';
            if (newVal < 0 || newVal > 9) {
                return INT_MIN;
            }
            val = val * 10 + newVal;
        } while (*(++integer) != 0);
        return val;
    }

    inline int tryParseInt(char *integer) {
        int val = 0;
        bool neg = false;
        if (*integer == '-') {
            neg = true;
            ++integer;
        }

        do {
            int newVal = *integer - '0';
            if (newVal < 0 || newVal > 9) {
                return INT_MIN;
            }
            val = val * 10 + newVal;
        } while (*(++integer) != 0);
        return neg ? -val : val;
    }

    inline int parseInt(char *integer) {
        int val = 0;
        bool neg = false;
        if (*integer == '-') {
            neg = true;
            ++integer;
        }

        do {
            val = val * 10 + *integer - '0';
        } while (*(++integer) != 0);
        return neg ? -val : val;
    }

    inline int parseUInt(char *integer) {
        int val = 0;

        do {
            val = val * 10 + *integer - '0';
        } while (*(++integer) != 0);
        return val;
    }

    const char HEX2DEC[256] =
            {
                    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
                    /* 0 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* 1 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* 2 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* 3 */  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,

                    /* 4 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* 5 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* 6 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* 7 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

                    /* 8 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* 9 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* A */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* B */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

                    /* C */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* D */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* E */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                    /* F */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
            };

    inline void uriDecode(char *pSrc) {
        // Note from RFC1630: "Sequences which start with a percent
        // sign but are not followed by two hexadecimal characters
        // (0-9, A-F) are reserved for future extension"
        char *pEnd = pSrc;
        while (*pSrc != 0) {
            if (*pSrc == '%') {
                char dec1, dec2;
                if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                    && -1 != (dec2 = HEX2DEC[*(pSrc + 2)])) {
                    *pEnd++ = ((dec1 << 4) | dec2);
                    pSrc += 3;
                    continue;
                }
            }
            if (*pSrc == '+') {
                *pEnd++ = ' ';
                ++pSrc;
            } else {
                *pEnd++ = *pSrc++;
            }
        }
        *pEnd = 0;
    }

    const short CYRILIC_CONVERT[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     -32304, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -28464, -28208, -27952, -27696, -27440, -27184, -26928, -26672, -26416,
                                     -26160, -25904, -25648, -25392, -25136, -24880, -24624, -24368, -24112, -23856, -23600, -23344, -23088, -22832, -22576,
                                     -22320, -22064, -21808, -21552, -21296, -21040, -20784, -20528, -20272, -20016, -19760, -19504, -19248, -18992, -18736,
                                     -18480, -18224, -17968, -17712, -17456, -17200, -16944, -16688, -16432, -32559, -32303, -32047, -31791, -31535, -31279,
                                     -31023, -30767, -30511, -30255, -29999, -29743, -29487, -29231, -28975, -28719, 0, -28207};

    inline void unicodeDecode(char *pSrc, char *pDest) {
        // Note from RFC1630: "Sequences which start with a percent
        // sign but are not followed by two hexadecimal characters
        // (0-9, A-F) are reserved for future extension"

        while (*pSrc != 0) {
            if (*pSrc == '\\' && (*(pSrc + 1) == 'u')) {
                short idx = 0;
                idx |= (HEX2DEC[*(pSrc + 2)] << 4) | HEX2DEC[*(pSrc + 3)];
                idx <<= 8;
                idx |= (HEX2DEC[*(pSrc + 4)] << 4) | HEX2DEC[*(pSrc + 5)];
                *((short *) pDest) = CYRILIC_CONVERT[idx];
                pDest += 2;
                pSrc += 6;
            } else {
                *pDest++ = *pSrc++;
            }
        }
        *pDest = 0;
    }

};


#endif //EPOLL_TRY_UTIL_H
