#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::ReadList(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long list_number = 0;
    if(info.Length() > 0) {
        list_number = info[0].As<Napi::Number>().Uint32Value();
    }

    long max_buffer_size = 500;
    char *buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len;

    long code;
    long count;

    do {
        ic_readlist(&list_number, buffer, &max_buffer_size, &buffer_len, &count, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Readlist failed. Code = %ld.\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}