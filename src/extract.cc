#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Extract(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long field_pos = 1;
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

    std::string record_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(record_string.c_str());
    const char *record = param.c_str();

    long record_len = strlen(record);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    do {
        ic_extract((char*)record, &record_len, &field_pos, &value_pos, &subvalue_pos, buffer, &max_buffer_size, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in extraction. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}