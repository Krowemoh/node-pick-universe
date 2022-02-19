#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::RunIType(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long record_id_len = strlen(record_id);

    std::string filename_string = info[1].ToString().Utf8Value();
    const char *filename = filename_string.c_str();
    long filename_len = strlen(filename);

    std::string itype_string = info[2].ToString().Utf8Value();
    const char *itype = itype_string.c_str();
    long itype_len = strlen(itype);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;

    do {
        ic_itype((char*)filename, &filename_len, (char*)record_id, &record_id_len, (char*)itype, &itype_len, buffer, &max_buffer_size, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in getting session info. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}