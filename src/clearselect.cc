#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::ClearSelect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long code;

    long list_number = 0;
    if(info.Length() > 0) {
        list_number = info[0].As<Napi::Number>().Uint32Value();
    }

    ic_clearselect(&list_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in clearing select file. Code (%ld)\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}