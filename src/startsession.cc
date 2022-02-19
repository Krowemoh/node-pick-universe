#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::StartSession(const Napi::CallbackInfo& info) {
    char *server_name = (char *)this->_host.c_str();
    char *user_name = (char *)this->_username.c_str();
    char *password = (char *)this->_password.c_str();
    char *account = (char *)this->_account.c_str();

    long code;

    long session_id = ic_universe_session(server_name, user_name, password, account, &code, NULL);

    this->_session_id = session_id;

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to open session. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    return info.Env().Null();
}