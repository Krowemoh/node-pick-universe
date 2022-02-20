#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::FileUnlock(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long status_func;
    long code;

    ic_fileunlock(&file_id, &status_func, &code);

    if (code != 0) {
        std::string error = "Error in unlocking file. Code (" + std::to_string(code) + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    if (status_func != 0) {
        Napi::TypeError::New(env, "File wasn't locked.").ThrowAsJavaScriptException();
        return env.Null();

    }

    return env.Null();
}