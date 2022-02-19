#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::FileLock(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long status_func;
    long code;

    ic_filelock(&file_id, &status_func, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in locking the file. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    if (status_func != 0) {
        char error[100];
        snprintf(error, 100, "Unable to get lock due to user : %ld", status_func);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    }

    return env.Null();
}