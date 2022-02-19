
#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::SetSession(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long session_id = info[0].As<Napi::Number>().Uint32Value();

    long code;
    ic_setsession(&session_id, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to set session. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    this->_session_id = session_id;
    return info.Env().Null();
}