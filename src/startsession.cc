#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::StartSession(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    char *server_name = (char *)this->_host.c_str();
    char *user_name = (char *)this->_username.c_str();
    char *password = (char *)this->_password.c_str();
    char *account = (char *)this->_account.c_str();

    long code;

    long session_id = ic_universe_session(server_name, user_name, password, account, &code, NULL);

    this->_session_id = session_id;

    if (code != 0) {
        std::string error = "Error in starting session. Code (" + std::to_string(code) + ")  - " + error_map[code];
        Napi::TypeError::New(env, "error").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return info.Env().Null();
}