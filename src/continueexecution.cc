#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::ContinueExecution(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    std::string reply_string = info[0].ToString().Utf8Value();
    const char *reply = reply_string.c_str();
    long reply_len = strlen(reply);

    long add_newline = 0;

    long buffer_len = 5000;
    char buffer[buffer_len];

    long text_len;
    long r1;
    long r2;
    long code;

    ic_inputreply((char*)reply, &reply_len, &add_newline, buffer, &buffer_len, &text_len, &r1, &r2, &code);

    std::string text;
    text.append(buffer);

    if (code == IE_BTS) {
        do {
            ic_executecontinue(buffer, &buffer_len, &text_len, &r1, &r2, &code);
            text.append(buffer, 0, text_len);
        } while (code == IE_BTS);
    }

    if (code != 0 && code != IE_AT_INPUT) {
        char error[100];
        snprintf(error, 100, "Error in continuing execution. Code: %ld, R1: (%ld), R2: (%ld).\n", code, r1, r2);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char *out = iso_8859_1_to_utf8((unsigned char*)text.c_str(), text.length());
    Napi::String output = Napi::String::New(env, (char*)out);
    Napi::String return_code = Napi::String::New(env, std::to_string(code).c_str());

    Napi::Array arguments = Napi::Array::New(env, 2);
    arguments[(int)0] = return_code;
    arguments[1] = output;

    free(out);
    return arguments;
}