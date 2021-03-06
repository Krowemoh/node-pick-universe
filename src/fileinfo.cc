#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::FileInfo(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long key = info[1].As<Napi::Number>().Uint32Value();

    long result;

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;

    do {
        ic_fileinfo(&key, &file_id, &result, buffer, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code == IE_STR) {
        unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
        free(buffer);
        Napi::String data = Napi::String::New(env, (char*)out);
        free(out);
        return data;

    } else {
        free(buffer);

        if (code != 0) {
            std::string error = "Error in fileinfo. Code (" + std::to_string(code) + ")  - " + error_map[code];
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::Number data = Napi::Number::New(env, result);
        return data;
    }
}
