#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::ReadValue(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long field_number = info[1].As<Napi::Number>().Uint32Value();

    long file_id = info[2].As<Napi::Number>().Uint32Value();

    long max_field_size = 500;
    char* field = (char*)malloc(max_field_size * sizeof(char));
    long field_len = 0;

    long lock = IK_READ;
    long status_func;
    long code;
    do {
        ic_readv(&file_id, &lock, (char*)record_id, &id_len, &field_number, field, &max_field_size, &field_len, &status_func, &code);
        if (code == IE_BTS) {
            free(field);
            max_field_size = max_field_size * 2;
            field = (char*)malloc(max_field_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(field);
        char error[500];
        snprintf(error, 500, "Record does not exist.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)field, field_len);
    free(field);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}