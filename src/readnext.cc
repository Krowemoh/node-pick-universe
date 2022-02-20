#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

Napi::Value Universe::ReadNext(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long list_number = 0;
    if(info.Length() > 0) {
        list_number = info[0].As<Napi::Number>().Uint32Value();
    }

    long code;
    long max_id_size = 300;
    char* record_id = (char*)malloc(max_id_size * sizeof(char));
    long id_len = 0;

    do {
        ic_readnext(&list_number, record_id, &max_id_size, &id_len, &code);

        if (code == IE_BTS) {
            free(record_id);
            max_id_size = max_id_size * 2;
            record_id = (char*)malloc(max_id_size * sizeof(char));
        }
    } while (code == IE_BTS);


    if (code == IE_LRR) {
        free(record_id);
        return env.Null();

    } else if (code != 0) {
        free(record_id);
        std::string error = "Error, No select list. Code (" + std::to_string(code) + ")";
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record_id, id_len);
    free(record_id);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}