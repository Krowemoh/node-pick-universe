#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::Lock(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
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
        std::string error = "Error in locking. Code (" + std::to_string(code) + ")  - " + error_map[code];
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}