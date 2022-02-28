#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::RecordLock(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string raw_record_id = info[0].ToString().Utf8Value();
    std::string record_id = UTF8toISO8859_1(raw_record_id.c_str());
    long id_len = record_id.length();

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long lock = IK_READL;
    if (info.Length() > 2) {
       lock = info[2].As<Napi::Number>().Uint32Value();
    }

    long status_func;
    long code;

    ic_recordlock(&file_id, &lock, record_id.data(), &id_len, &status_func, &code);

    if (code != 0) {
        Napi::TypeError::New(env, "Failed to get a recordlock.").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Number data = Napi::Number::New(env, status_func);
    return data;
}
