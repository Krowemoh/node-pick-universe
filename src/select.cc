#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Select(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();

    long list_number = 0;
    if(info.Length() > 1) {
        list_number = info[1].As<Napi::Number>().Uint32Value();
    }

    long code;
    ic_select(&file_id, &list_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Select failed. Code = %ld.\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    return env.Null();
}