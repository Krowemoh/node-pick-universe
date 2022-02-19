#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::EndAllSessions(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_quitall(&code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to close sessions. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    this->_session_id = 0;
    return info.Env().Null();
}