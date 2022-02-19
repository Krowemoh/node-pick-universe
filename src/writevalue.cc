#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value WriteValueBase(const Napi::CallbackInfo& info, long lock_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    std::string field_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(field_string.c_str());
    const char *field = param.c_str();
    long field_len = strlen(field);

    std::string record_id_string = info[1].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long field_number = info[2].As<Napi::Number>().Uint32Value();

    long file_id = info[3].As<Napi::Number>().Uint32Value();

    long status_func;
    long code;

    ic_writev(&file_id, &lock_type, (char*)record_id, &id_len, &field_number, (char*)field, &field_len, &status_func, &code);

    if (code == IE_LCK) {
        char error[100];
        snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code != 0) {
        char error[200];
        snprintf(error, 200, "Error in writing value record. Code (%ld), Status (%ld). Record: %s\n", code, status_func, record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return info.Env().Null();
}

Napi::Value Universe::WriteValue(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException(); 
        return env.Null();
    }

    long lock_type = IK_WRITE;
    return WriteValueBase(info, lock_type);
}
