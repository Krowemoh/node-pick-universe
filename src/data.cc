#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Data(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string pre_param = info[0].ToString().Utf8Value();
    const char * c_param = pre_param.c_str();
    std::string param = UTF8toISO8859_1(c_param);

    const char* data = param.c_str();

    long data_len = strlen(data);

    long code;
    ic_data((char*)data, &data_len, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in adding input. Code (%ld)\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}
