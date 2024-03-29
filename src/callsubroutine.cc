#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "intcall.h"
#include <string.h>
#include <ctype.h>
#include <ffi.h>
#include <locale.h>

#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

using namespace Napi;

long call_subroutine(const char *subname, long numargs, ICSTRING *icList, long &code) {
    int pad = 4;
    int arg_len = pad + numargs;

    ffi_cif     call_interface;
    ffi_type    *ret_type;
    ffi_type    *arg_types[arg_len];

    ret_type = &ffi_type_slong;

    arg_types[0] = &ffi_type_pointer;
    arg_types[1] = &ffi_type_pointer;
    arg_types[2] = &ffi_type_pointer;
    arg_types[3] = &ffi_type_pointer;

    for (int i=0;i <numargs; i++) {
        arg_types[pad+i] = &ffi_type_pointer;
    }

    if (ffi_prep_cif(&call_interface, FFI_DEFAULT_ABI, arg_len, ret_type, arg_types) == FFI_OK) {
        void *arg_values[arg_len];

        const char **subname_pointer = &subname;
        arg_values[0] = subname_pointer;

        long size = strlen(subname);
        long * size_pointer = &size;
        arg_values[1] = &size_pointer;

        long *code_pointer = &code;
        arg_values[2] = &code_pointer;

        long * numargs_pointer = &numargs;
        arg_values[3] = &numargs_pointer;

        ICSTRING *ptrs[numargs];

        for (int i=0;i <numargs; i++) {
            ptrs[i] = &icList[i];
            arg_values[pad+i] = &ptrs[i];
        }

        long z = 0;
        ffi_call(&call_interface, FFI_FN(ic_subcall), &z, arg_values);
        return z;
    }
    return -1;
}

Napi::Value Universe::CallSubroutine(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    int MAX_ARGS = info.Length() -1;
    ICSTRING icList[MAX_ARGS];

    for (int i=0;i<MAX_ARGS;i++) {
        std::string pre_param = info[i+1].ToString().Utf8Value();
        const char * c_param = pre_param.c_str();
        std::string param = UTF8toISO8859_1(c_param);

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

    std::string subname = info[0].ToString().Utf8Value();

    long code = 0;
    call_subroutine(subname.data(), MAX_ARGS, icList, code);

    if (code != 0) {
        for (int i=0;i<MAX_ARGS;i++) {
            if (icList[i].len >0) {
                ic_free(icList[i].text);
            }
        }
        std::string error = "Error in calling subroutine. Code (" + std::to_string(code) + ")  - " + error_map[code];
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Array arguments = Napi::Array::New(env, info.Length()-1);
    for (int i=0;i<MAX_ARGS;i++) {
        unsigned char *in = icList[i].text;
        unsigned char *out = iso_8859_1_to_utf8(in, icList[i].len);
        Napi::String data = Napi::String::New(env, (char*)out);
        arguments[i] = data;
        free(out);
        if (icList[i].len >0) {
            ic_free(icList[i].text);
        }
    }
    return arguments;
}
