#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value WriteBase(const Napi::CallbackInfo& info, long lock_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    std::string record_id_string = info[1].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long file_id = info[2].As<Napi::Number>().Uint32Value();

    std::string record_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(record_string.c_str());

    const char *record = param.c_str();
    long record_len = strlen(record);

    long status_func;
    long code;

    ic_write(&file_id, &lock_type, (char*)record_id, &id_len, (char*)record, &record_len, &status_func, &code);

    if (code == IE_LCK) {
        char error[100];
        snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in writing record. Code (%ld), Status (%ld). Record: %s\n", code, status_func, record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return info.Env().Null();
}

Napi::Value Universe::Write(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_type = IK_WRITE;
    return WriteBase(info, lock_type);
}
