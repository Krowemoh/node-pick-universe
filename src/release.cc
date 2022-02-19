#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Release(const Napi::CallbackInfo& info) {
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

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long code;
    ic_release(&file_id, (char*)record_id, &id_len, &code);

    if (code != 0) {
        char error[500];
        snprintf(error, 500, "Failed to get a recordlock on the record.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}