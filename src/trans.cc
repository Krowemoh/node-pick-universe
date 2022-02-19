#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Trans(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    std::string filename_string = info[1].ToString().Utf8Value();
    const char *filename = filename_string.c_str();
    long file_len = strlen(filename);

    long field_pos = -1;
    if (info.Length() > 2) {
        field_pos = info[2].As<Napi::Number>().Uint32Value();
    }

    long dict_flag = IK_DATA;

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    const char *control_code = "X";
    long control_code_len = strlen(control_code);

    long status_func;
    long code;

    do {
        ic_trans((char*)filename, &file_len, &dict_flag, (char*)record_id, &id_len, &field_pos, (char *)control_code, &control_code_len, buffer, &max_buffer_size, &buffer_len, &status_func, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[500];
        snprintf(error, 500, "Error in reading record. Code (%ld), Status (%ld).\n", code, status_func);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}
