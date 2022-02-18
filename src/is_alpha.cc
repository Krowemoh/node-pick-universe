#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "intcall.h"
#include <string.h>
#include <ctype.h>
#include <ffi.h>
#include <locale.h>

#include "convert.h"
#include "universe.h"

using namespace Napi;

Napi::Value Universe::IsAlpha(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string temp = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(temp.c_str());

    const char *query = param.c_str();
    long query_len = strlen(query);

    long code;
    ic_alpha((char *)query, &query_len, &code);

    Napi::Env env = info.Env();
    return Napi::Number::New(env, code);
}
