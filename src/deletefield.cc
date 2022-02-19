#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::DeleteField(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string record_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(record_string.c_str());
    const char *record = param.c_str();
    long record_len = strlen(record);

    long field_pos = 0;
    if (info.Length() > 1) {
        field_pos = info[1].As<Napi::Number>().Uint32Value();
    }

    long value_pos = 0;
    if (info.Length() > 2) {
        value_pos = info[2].As<Napi::Number>().Uint32Value();
    }

    long subvalue_pos = 0;
    if (info.Length() > 3) {
        subvalue_pos = info[3].As<Napi::Number>().Uint32Value();
    }

    long code;
    ic_strdel((char*)record, &record_len, &field_pos, &value_pos, & subvalue_pos, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in removing field. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record, record_len);
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}