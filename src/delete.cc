#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value DeleteBase(const Napi::CallbackInfo& info, long lock_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    std::string raw_record_id = info[0].ToString().Utf8Value();
    std::string record_id = UTF8toISO8859_1(raw_record_id.c_str());
    long id_len = record_id.length();

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long status_func;
    long code;

    ic_delete(&file_id, &lock_type, record_id.data(), &id_len, &status_func, &code);

    if (code == IE_RNF) {
        std::string error = "Record not found. Record (" + record_id + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code == IE_LCK) {
        std::string error = "Record is locked. Record (" + record_id + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code != 0) {
        std::string error = "Error in deleting record. ";
        error += "Code (" + std::to_string(code) + "), ";
        error += "Status (" + std::to_string(status_func) + "), ";
        error += "Record ID (" + record_id + ")";

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
