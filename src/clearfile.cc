#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::ClearFile(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long code;

    ic_clearfile(&file_id, &code);

    if (code == IE_ENOENT) {
        char error[100];
        snprintf(error, 100, "No such file or directory to clear. Code (%ld).", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    } else if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in clearing file. Code (%ld)\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}