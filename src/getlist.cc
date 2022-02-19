#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::GetList(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string list_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(list_string.c_str());
    const char *list_c = param.c_str();
    long list_len = strlen(list_c);

    long list_number = 0;
    if (info.Length() > 1) {
        list_number = info[1].As<Napi::Number>().Uint32Value();
    }

    long code;
    ic_getlist((char*)list_c, &list_len, &list_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in getting list. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    return env.Null();
}