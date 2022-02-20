#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::DeleteField(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string record_string = info[0].ToString().Utf8Value();
    std::string record = UTF8toISO8859_1(record_string.c_str());
    long record_len = record.length();

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
    ic_strdel(record.data(), &record_len, &field_pos, &value_pos, & subvalue_pos, &code);

    if (code != 0) {
        std::string error = "Error in deleting field. Code (" + std::to_string(code) + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record.data(), record_len);
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}