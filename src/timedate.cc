#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::TimeDate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long max_buffer_size = 21;
    char buffer[max_buffer_size];
    long buffer_len;

    long code;
    ic_timedate(buffer, &max_buffer_size, &buffer_len, &code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to get time. Code = %ld\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}