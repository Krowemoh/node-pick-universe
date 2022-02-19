#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::SetTimeout(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long timeout = info[0].As<Napi::Number>().Uint32Value();

    long code;
    ic_set_comms_timeout(&timeout, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in setting timeout. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}