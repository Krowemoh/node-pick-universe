#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::ICONV(const Napi::CallbackInfo& info) {
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

    std::string conversion_string = info[1].ToString().Utf8Value();
    std::string conversion_param = UTF8toISO8859_1(conversion_string.c_str());
    const char *conversion = conversion_param.c_str();
    long conversion_len = strlen(conversion);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;
    do {
        ic_iconv((char*)conversion, &conversion_len, (char*)record, &record_len, buffer, &max_buffer_size, &buffer_len, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0 && code != 1 && code != 2 && code != 3) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in iconv. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}