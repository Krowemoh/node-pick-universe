#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::Locate(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string search_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(search_string.c_str());
    const char *search = param.c_str();
    long search_len = strlen(search);

    std::string haystack_string = info[1].ToString().Utf8Value();
    std::string haystack_param = UTF8toISO8859_1(haystack_string.c_str());
    const char *haystack = haystack_param.c_str();
    long haystack_len = strlen(haystack);

    std::string order_string = info[2].ToString().Utf8Value();
    std::string order_param = UTF8toISO8859_1(order_string.c_str());
    const char *order = order_param.c_str();
    long order_len = strlen(order);

    long start = 1;
    if (info.Length() > 3) {
        start = info[3].As<Napi::Number>().Uint32Value();
    }

    long field_pos = 0;
    if (info.Length() > 4) {
        field_pos = info[4].As<Napi::Number>().Uint32Value();
    }

    long value_pos = 0;
    if (info.Length() > 5) {
        value_pos = info[5].As<Napi::Number>().Uint32Value();
    }

    long index;
    long found;
    long code;

    ic_locate((char*)search, &search_len, (char*)haystack, &haystack_len, &field_pos, &value_pos, &start, (char*)order, &order_len, &index, &found, &code);

    if (code != 0) {
        std::string error = "Error in locate. Code (" + std::to_string(code) + ")  - " + error_map[code];
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Array arguments = Napi::Array::New(env, 2);
    arguments[(int)0] = found;
    arguments[1] = index;
    return arguments;
}