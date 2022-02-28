#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::Open(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long file_id;
    long code;

    std::string filename = info[0].ToString().Utf8Value();

    long file_len = filename.length();
    long status_func;
    long universe_file_type = IK_DATA;

    ic_open(&file_id, &universe_file_type, filename.data(), &file_len, &status_func, &code);

    if (code == IE_ENOENT) {
        std::string error = "No such file or directory. ";
        error += "Code (" + std::to_string(code) + "), ";
        error += "Filename (" + filename + ")";

        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    } else if (code != 0 || file_id == 0 || status_func <= 0) {
        std::string error = "Error in opening file. ";
        error += "Code (" + std::to_string(code) + ")  - " + error_map[code];
        error += "Status (" + std::to_string(status_func) + ")";
        error += "Filename (" + filename + ")";

        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Number data = Napi::Number::New(env, file_id);
    return data;
}