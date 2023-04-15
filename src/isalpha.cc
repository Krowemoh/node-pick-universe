#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::IsAlpha(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string temp = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(temp.c_str());

    long query_len = param.length();

    long code;
    ic_alpha((char*)param.data(), &query_len, &code);

    if (code > 1) {
        std::string error = "Error in checking alpha. Code (" + std::to_string(code) + ")  - " + error_map[code];
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, code);
}
