#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::SelectIndex(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string index_string = info[0].ToString().Utf8Value();
    const char* ak_name = index_string.c_str();
    long ak_len = strlen(ak_name);

    std::string value_string = info[1].ToString().Utf8Value();
    const char* ak_value = value_string.c_str();
    long ak_value_len = strlen(ak_value);

    long file_id = info[2].As<Napi::Number>().Uint32Value();

    long list_number = 0;
    if(info.Length() > 3) {
        list_number = info[3].As<Napi::Number>().Uint32Value();
    }

    long status_func;
    long code;
    ic_selectindex(&file_id, &list_number, (char*)ak_name, &ak_len, (char*)ak_value, &ak_value_len, &status_func, &code);

    if (code != 0) {
        std::string error = "Error in selectindex. Code (" + std::to_string(code) + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    if (status_func != 0) {
        std::string error = "Failed selectindex. Status (" + std::to_string(status_func) + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}