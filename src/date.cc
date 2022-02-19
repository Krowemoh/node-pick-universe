#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Date(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long date;
    long code;
    ic_date(&date, &code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to get date. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Env env = info.Env();
    return Napi::Number::New(env, date);
}