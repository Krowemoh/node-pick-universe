#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Lock(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_number = info[0].As<Napi::Number>().Uint32Value();
    if (lock_number < 0 || lock_number > 63) {
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, "Invalid lock.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_lock(&lock_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to get lock. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}