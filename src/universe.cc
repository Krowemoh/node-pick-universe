#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "intcall.h"
#include <string.h>
#include <ctype.h>
#include <ffi.h>
#include <locale.h>

#include "universe.h"

using namespace Napi;

// https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
unsigned char* iso_8859_1_to_utf8(unsigned char *in, long max) {
    unsigned char *out = (unsigned char *) malloc(2*(max+1));

    if (max == 0) {
        out[0] = '\0';
        return out;
    }

    long i = 0;
    long j = 0;
    while (j < max) {
        if (*in<128) {
            out[i++] = *in++;
        } else {
            out[i++] = 0xc2 + (*in > 0xbf); 
            out[i++] = (*in++ & 0x3f) + 0x80;
        }
        j++;
    }
    out[i] = '\0';
    return out;
}

// https://stackoverflow.com/questions/23689733/convert-string-from-utf-8-to-iso-8859-1
std::string UTF8toISO8859_1(const char * in) {
    std::string out;
    if (in == NULL)
        return out;

    unsigned int codepoint = 0;
    while (*in != 0) {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch <= 0x7f)
            codepoint = ch;
        else if (ch <= 0xbf)
            codepoint = (codepoint << 6) | (ch & 0x3f);
        else if (ch <= 0xdf)
            codepoint = ch & 0x1f;
        else if (ch <= 0xef)
            codepoint = ch & 0x0f;
        else
            codepoint = ch & 0x07;
        ++in;
        if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
        {
            if (codepoint <= 255)
            {
                out.append(1, static_cast<char>(codepoint));
            }
            else
            {
                // do whatever you want for out-of-bounds characters
            }
        }
    }
    return out;
}

long call_subroutine(char *subname, long numargs, ICSTRING *icList) {
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

        char **subname_pointer = &subname;
        arg_values[0] = subname_pointer;

        long size = strlen(subname);
        long * size_pointer = &size;
        arg_values[1] = &size_pointer;

        long status = 0;
        long * status_pointer = &status;
        arg_values[2] = &status_pointer;

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

        std::string name = info[0].ToString().Utf8Value();
        char *subname = (char *)name.c_str();

        long sub_status = call_subroutine(subname, MAX_ARGS, icList);

        ic_quit(&code);
        if (code != 0) {
            fprintf(stderr, "Failed to close session. Code = %ld\n", code);
        }

        if (false) {
            if (sub_status != 0) {
                char error[100];
                snprintf(error, 100, "Failed to complete subroutine. Code = %ld\n", sub_status);
                Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
                return env.Null();
            }
        }
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

Napi::Value Universe::start_session(const Napi::CallbackInfo& info) {
    char *server_name = (char *)this->_host.c_str();
    char *user_name = (char *)this->_username.c_str();
    char *password = (char *)this->_password.c_str();
    char *account = (char *)this->_account.c_str();

    long code;

    ic_universe_session(server_name, user_name, password, account, &code, NULL);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to open session. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    return info.Env().Null();
}

Napi::Value ReadBase(const Napi::CallbackInfo& info, long universe_file_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();
    long file_id;
    unsigned char *out;
    long code;

    std::string filename_string = info[1].ToString().Utf8Value();
    const char *filename = filename_string.c_str();

    long file_len = strlen(filename);
    long status_func;

    ic_open(&file_id, &universe_file_type, (char *)filename, &file_len, &status_func, &code);

    long lock = IK_READ;

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);
    long max_rec_size = 300;

    char* record = (char*)malloc(max_rec_size * sizeof(char));
    long record_len = 0;

    do {
        ic_read(&file_id, &lock, (char*)record_id, &id_len, record, &max_rec_size, &record_len, &status_func, &code);

        if (code == IE_BTS) {
            free(record);
            max_rec_size = max_rec_size * 2;
            record = (char*)malloc(max_rec_size * sizeof(char));

        } else if (status_func != 0) {
            free(record);
            char error[100];
            snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();

        } else if (record_len == 0) {
            free(record);
            char error[100];
            snprintf(error, 100, "Record does not exist. Record: %s\n", record_id);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
        }
    } while (code == IE_BTS);

    out = iso_8859_1_to_utf8((unsigned char*)record, record_len);
    free(record);

    ic_close(&file_id, &code);

    ic_quit(&code);
    if (code != 0) {
        fprintf(stderr, "Failed to close session. Code = %ld\n", code);
    }

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::Read(const Napi::CallbackInfo& info) {
    Universe::start_session(info);
    return ReadBase(info, IK_DATA);
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

Napi::Function Universe::GetClass(Napi::Env env) {
    return DefineClass(env, "Universe", {
            Universe::InstanceMethod("CallSubroutine", &Universe::CallSubroutine),
            Universe::InstanceMethod("Read", &Universe::Read),
            });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "Universe");
    exports.Set(name, Universe::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
