#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "universe.h"

Napi::Value Universe::Time(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long time;
    long code;
    ic_time(&time, &code);
    
    if (code != 0) {
        std::string error = "Error in getting time. Code (" + std::to_string(code) + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, time);
}
