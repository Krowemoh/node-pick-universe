#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Read(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long lock = IK_READ;
    long status_func;
    long code;

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);
    long max_rec_size = 500;

    char* record = (char*)malloc(max_rec_size * sizeof(char));
    long record_len = 0;

    do {
        ic_read(&file_id, &lock, (char*)record_id, &id_len, record, &max_rec_size, &record_len, &status_func, &code);

        if (code == IE_BTS) {
            free(record);
            max_rec_size = max_rec_size * 2;
            record = (char*)malloc(max_rec_size * sizeof(char));

        } else if (status_func != 0) {
            free(record);
            char error[100];
            snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();

        } else if (record == NULL && record_len == 0) {
            char error[100];
            snprintf(error, 100, "Record does not exist. Record: %s\n", record_id);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(record);
        char error[500];
        snprintf(error, 500, "Failed to read record.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record, record_len);
    free(record);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}
