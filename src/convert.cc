#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "convert.h"

// https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
unsigned char* iso_8859_1_to_utf8(unsigned char *in, long max) {
    unsigned char *out = (unsigned char *) malloc(2*(max+1));

    if (max == 0) {
        out[0] = '\0';
        return out;
    }

    long i = 0;
    long j = 0;
    while (j < max) {
        if (*in<128) {
            out[i++] = *in++;
        } else {
            out[i++] = 0xc2 + (*in > 0xbf); 
            out[i++] = (*in++ & 0x3f) + 0x80;
        }
        j++;
    }
    out[i] = '\0';
    return out;
}

// https://stackoverflow.com/questions/23689733/convert-string-from-utf-8-to-iso-8859-1
std::string UTF8toISO8859_1(const char * in) {
    std::string out;
    if (in == NULL)
        return out;

    unsigned int codepoint = 0;
    while (*in != 0) {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch <= 0x7f)
            codepoint = ch;
        else if (ch <= 0xbf)
            codepoint = (codepoint << 6) | (ch & 0x3f);
        else if (ch <= 0xdf)
            codepoint = ch & 0x1f;
        else if (ch <= 0xef)
            codepoint = ch & 0x0f;
        else
            codepoint = ch & 0x07;
        ++in;
        if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
        {
            if (codepoint <= 255)
            {
                out.append(1, static_cast<char>(codepoint));
            }
            else
            {
                // do whatever you want for out-of-bounds characters
            }
        }
    }
    return out;
}


