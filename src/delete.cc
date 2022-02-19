#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value DeleteBase(const Napi::CallbackInfo& info, long lock_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long status_func;
    long code;

    ic_delete(&file_id, &lock_type, (char*)record_id, &id_len, &status_func, &code);

    if (code == IE_RNF) {
        char error[100];
        snprintf(error, 100, "Record not found. Record: %s\n",  record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code == IE_LCK) {
        char error[100];
        snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in deleting record. Code (%ld), Status (%ld). Record: %s\n", code, status_func, record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return info.Env().Null();
}

Napi::Value Universe::Delete(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_type = IK_DELETE;
    return DeleteBase(info, lock_type);
}