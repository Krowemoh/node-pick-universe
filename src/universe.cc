#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "intcall.h"
#include <string.h>
#include <ctype.h>
#include <ffi.h>

#include "universe.h"

using namespace Napi;

double call_subroutine(char *subname, long numargs, ICSTRING *icList) {
    int pad = 4;
    int arg_len = pad + numargs;

    ffi_cif     call_interface;
    ffi_type    *ret_type;
    ffi_type    *arg_types[arg_len];

    ret_type = &ffi_type_double;

    ffi_type icstring_type;
    ffi_type *icstring_type_elements[3];

    icstring_type.size = 0;
    icstring_type.alignment = -1;
    icstring_type.type = FFI_TYPE_STRUCT;
    icstring_type.elements = icstring_type_elements;

    icstring_type_elements[0] = &ffi_type_slong;
    icstring_type_elements[1] = &ffi_type_pointer;
    icstring_type_elements[2] = NULL;

    arg_types[0] = &ffi_type_pointer;
    arg_types[1] = &ffi_type_pointer;
    arg_types[2] = &ffi_type_pointer;
    arg_types[3] = &ffi_type_pointer;

        arg_types[4] = &icstring_type;
        arg_types[5] = &icstring_type;
        arg_types[6] = &icstring_type;

    if (ffi_prep_cif(&call_interface, FFI_DEFAULT_ABI, arg_len, ret_type, arg_types) == FFI_OK) {
        void *arg_values[arg_len];

        char **subname_pointer = &subname;
        arg_values[0] = subname_pointer;

        long size = strlen(subname);
        long * size_pointer = &size;
        arg_values[1] = &size_pointer;

        long sub_status = 0;
        long * sub_status_pointer = &sub_status;
        arg_values[2] = &sub_status_pointer;

        long * numargs_pointer = &numargs;
        arg_values[3] = &numargs_pointer;

        ICSTRING *ptrs[numargs];
        ptrs[0] = &icList[0];
        ptrs[1] = &icList[1];
        ptrs[2] = &icList[2];

        arg_values[4] = &ptrs[0];

        arg_values[5] = &ptrs[2];
        arg_values[6] = &ptrs[1];

        double z = 0;
        //ic_subcall(*subname_pointer, size_pointer, sub_status_pointer, numargs_pointer, ptrs[0], ptrs[1], ptrs[2]);
        ffi_call(&call_interface, FFI_FN(ic_subcall), &z, arg_values);
        return z;
    }
    return -1;
}

Universe::Universe(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if ((int)info.Length() != 4) {
        Napi::TypeError::New(env, "Wrong number of arguments (host, username, password, account).")
            .ThrowAsJavaScriptException();
        return;
    }

    for (int i=0;i <(int)info.Length();i++) {
        if (!info[i].IsString()) {
            char error[100];
            snprintf(error, 100, "Argument %d isn't a string.\n", i);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return;
        }
    }

    this->_host = info[0].As<Napi::String>().Utf8Value();
    this->_username = info[1].As<Napi::String>().Utf8Value();
    this->_password = info[2].As<Napi::String>().Utf8Value();
    this->_account = info[3].As<Napi::String>().Utf8Value();
}

Napi::Value Universe::CallSubroutine(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    for (int i=0;i <(int)info.Length();i++) {
        if (!info[i].IsString()) {
            char error[100];
            snprintf(error, 100, "Argument %d isn't a string.\n", i);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    char *server_name = (char *)this->_host.c_str();
    char *user_name = (char *)this->_username.c_str();
    char *password = (char *)this->_password.c_str();
    char *account = (char *)this->_account.c_str();

    long code;
    ic_universe_session(server_name, user_name, password, account, &code, NULL);

    int MAX_ARGS = info.Length() -1;
    ICSTRING icList[MAX_ARGS];

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to open session. Code = %ld\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    } else {
        for (int i=0;i<MAX_ARGS;i++) {
            std::string param = info[i+1].ToString().Utf8Value();

            if (param == "") {
                const char *text = "";
                long size = strlen(text);
                icList[i].len = size;
                icList[i].text = (unsigned char*)ic_calloc(&size);
                memcpy(icList[i].text, text, icList[i].len);
            } else {
                char *text = (char *)param.c_str();
                long size = strlen(text);
                icList[i].len = size;
                icList[i].text = (unsigned char*)ic_calloc(&size);
                memcpy(icList[i].text, text, icList[i].len);
            }
        }

        std::string name = info[0].ToString().Utf8Value();
        char *subname = (char *)name.c_str();

        long sub_status = call_subroutine(subname, MAX_ARGS, icList);

        ic_quit(&code);
        if (code != 0) {
            fprintf(stderr, "Failed to close session. Code = %ld\n", code);
        }

        if (sub_status != 0) {
            char error[100];
            snprintf(error, 100, "Failed to complete subroutine. Code = %ld\n", sub_status);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    Napi::Array arguments = Napi::Array::New(env, info.Length()-1);
    for (int i=0;i<MAX_ARGS;i++) {
        const char *x = (const char*)icList[i].text;
        Napi::String data = Napi::String::New(env,x);
        arguments[i] = data;
    }

    for (int i=0; i<MAX_ARGS; i++) {
        if (icList[i].len >0) {
            ic_free(icList[i].text);
        }
    }

    return arguments;
}

Napi::Function Universe::GetClass(Napi::Env env) {
    return DefineClass(env, "Universe", {
            Universe::InstanceMethod("CallSubroutine", &Universe::CallSubroutine),
            });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "Universe");
    exports.Set(name, Universe::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
