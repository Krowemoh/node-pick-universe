#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::EndAllSessions(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_quitall(&code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to close sessions. Code = %ld\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    this->_session_id = 0;
    return env.Null();
}