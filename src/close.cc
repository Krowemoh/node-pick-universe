#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Close(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long code;

    ic_close(&file_id, &code);

    return env.Null();
}