#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::Raise(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string temp = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(temp.c_str());

    const char *buffer = param.c_str();
    long buffer_len = strlen(buffer);

    long code;
    ic_raise((char *)buffer, &buffer_len, &code);

    if (code != 0) {
        std::string error = "Error in raising. Code (" + std::to_string(code) + ")  - " + error_map[code];
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);

    return data;
}