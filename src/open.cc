#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::Open(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long file_id;
    long code;

    std::string filename_string = info[0].ToString().Utf8Value();
    const char *filename = filename_string.c_str();

    long file_len = strlen(filename);
    long status_func;
    long universe_file_type = IK_DATA;

    ic_open(&file_id, &universe_file_type, (char *)filename, &file_len, &status_func, &code);

    if (code == IE_ENOENT) {
        char error[100];
        snprintf(error, 100, "No such file or directory (%ld). Filename: %s\n", code, filename);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    } else if (code != 0 || file_id == 0 || status_func <= 0) {
        char error[100];
        snprintf(error, 100, "Error in opening file code (%ld), status (%ld). Filename: %s\n", code, status_func, filename);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Number data = Napi::Number::New(env, file_id);
    return data;
}