#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Date(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
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

    return Napi::Number::New(env, date);
}