
#include <stdlib.h>
#include <string.h>

#include "intcall.h"
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
        std::string error = "Error in setting session. Code (" + std::to_string(code) + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    this->_session_id = session_id;
    return info.Env().Null();
}