#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Lower(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string temp = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(temp.c_str());

    const char *buffer = param.c_str();
    long buffer_len = strlen(buffer);

    long code;
    ic_lower((char *)buffer, &buffer_len, &code);

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    Napi::Env env = info.Env();
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);

    return data;
}