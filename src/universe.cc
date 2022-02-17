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

    std::string name = info[0].ToString().Utf8Value();
    char *subname = (char *)name.c_str();

    long sub_status = call_subroutine(subname, MAX_ARGS, icList);

    if (false) {
        if (sub_status != 0) {
            char error[100];
            snprintf(error, 100, "Failed to complete subroutine. Code = %ld\n", sub_status);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
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

Napi::Value Universe::Data(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string pre_param = info[0].ToString().Utf8Value();
    const char * c_param = pre_param.c_str();
    std::string param = UTF8toISO8859_1(c_param);

    const char* data = param.c_str();

    long data_len = strlen(data);

    long code;
    ic_data((char*)data, &data_len, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in adding input. Code (%ld)\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Value Universe::ClearData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_cleardata(&code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in adding input. Code (%ld)\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Value Universe::Read(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long lock = IK_READ;
    long status_func;
    long code;

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);
    long max_rec_size = 500;

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

        } else if (record == NULL && record_len == 0) {
            char error[100];
            snprintf(error, 100, "Record does not exist. Record: %s\n", record_id);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
        }
    } while (code == IE_BTS);

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record, record_len);
    free(record);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value WriteBase(const Napi::CallbackInfo& info, long lock_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    std::string record_id_string = info[1].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long file_id = info[2].As<Napi::Number>().Uint32Value();

    std::string record_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(record_string.c_str());

    const char *record = param.c_str();
    long record_len = strlen(record);

    long status_func;
    long code;

    ic_write(&file_id, &lock_type, (char*)record_id, &id_len, (char*)record, &record_len, &status_func, &code);

    if (code == IE_LCK) {
        char error[100];
        snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in writing record. Code (%ld), Status (%ld). Record: %s\n", code, status_func, record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return info.Env().Null();
}

Napi::Value Universe::Write(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_type = IK_WRITE;
    return WriteBase(info, lock_type);
}

Napi::Value DeleteBase(const Napi::CallbackInfo& info, long lock_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long status_func;
    long code;

    ic_delete(&file_id, &lock_type, (char*)record_id, &id_len, &status_func, &code);

    if (code == IE_RNF) {
        char error[100];
        snprintf(error, 100, "Record not found. Record: %s\n",  record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code == IE_LCK) {
        char error[100];
        snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in deleting record. Code (%ld), Status (%ld). Record: %s\n", code, status_func, record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return info.Env().Null();
}

Napi::Value Universe::Delete(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_type = IK_DELETE;
    return DeleteBase(info, lock_type);
}

Napi::Value Universe::ReadNext(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
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
        char error[100];
        snprintf(error, 100, "Select failed. Code = %ld.\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record_id, id_len);
    free(record_id);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::Select(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();

    long list_number = 0;
    if(info.Length() > 1) {
        list_number = info[1].As<Napi::Number>().Uint32Value();
    }

    long code;
    ic_select(&file_id, &list_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Select failed. Code = %ld.\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    return env.Null();
}

Napi::Value Universe::ClearSelect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;

    long list_number = 0;
    if(info.Length() > 0) {
        list_number = info[0].As<Napi::Number>().Uint32Value();
    }

    ic_clearselect(&list_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in clearing select file. Code (%ld)\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Value Universe::Open(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
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

Napi::Value Universe::Close(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long code;

    ic_close(&file_id, &code);

    return env.Null();
}

Napi::Value Universe::ClearFile(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long code;

    ic_clearfile(&file_id, &code);

    if (code == IE_ENOENT) {
        char error[100];
        snprintf(error, 100, "No such file or directory to clear. Code (%ld).", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    } else if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in clearing file. Code (%ld)\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

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

Napi::Value Universe::EndSession(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_quit(&code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to close session. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    this->_session_id = 0;
    return info.Env().Null();
}

Napi::Value Universe::Date(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long date;
    long code;
    ic_date(&date, &code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to get date. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Env env = info.Env();
    return Napi::Number::New(env, date);
}

Napi::Value Universe::IsAlpha(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string temp = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(temp.c_str());

    const char *query = param.c_str();
    long query_len = strlen(query);

    long code;
    ic_alpha((char *)query, &query_len, &code);

    Napi::Env env = info.Env();
    return Napi::Number::New(env, code);
}

Napi::Value Universe::Extract(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long field_pos = 1;
    if (info.Length() > 1) {
        field_pos = info[1].As<Napi::Number>().Uint32Value();
    }

    long value_pos = 0;
    if (info.Length() > 2) {
        value_pos = info[2].As<Napi::Number>().Uint32Value();
    }

    long subvalue_pos = 0;
    if (info.Length() > 3) {
        subvalue_pos = info[3].As<Napi::Number>().Uint32Value();
    }

    long code;

    std::string record_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(record_string.c_str());
    const char *record = param.c_str();

    long record_len = strlen(record);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    do {
        ic_extract((char*)record, &record_len, &field_pos, &value_pos, &subvalue_pos, buffer, &max_buffer_size, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in extraction. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::Execute(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    std::string command_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(command_string.c_str());
    const char *command = param.c_str();

    long command_len = strlen(command);

    long buffer_len = 5000;
    char buffer[buffer_len];

    long text_len;
    long r1;
    long r2;
    long code;

    ic_execute((char*)command, &command_len, buffer, &buffer_len, &text_len, &r1, &r2, &code);

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
        snprintf(error, 100, "Error in execution. Code: %ld, R1: (%ld), R2: (%ld). Command: %s\n", code, r1, r2, command);
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

Napi::Value Universe::FileInfo(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long key = info[1].As<Napi::Number>().Uint32Value();

    printf("FileID: %ld\n", file_id);

    long result;

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;

    do {
        ic_fileinfo(&key, &file_id, &result, buffer, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code == IE_STR) {
        unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
        free(buffer);
        Napi::String data = Napi::String::New(env, (char*)out);
        free(out);
        return data;

    } else {
        free(buffer);

        if (code != 0) {
            char error[100];
            snprintf(error, 100, "Error in fileinfo. Code: %ld", code);
            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::Number data = Napi::Number::New(env, result);
        return data;
    }
}

Napi::Value Universe::FileLock(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long status_func;
    long code;

    ic_filelock(&file_id, &status_func, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in locking the file. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    if (status_func != 0) {
        char error[100];
        snprintf(error, 100, "Unable to get lock due to user : %ld", status_func);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    }

    return env.Null();
}

Napi::Value Universe::FileUnlock(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();
    long status_func;
    long code;

    ic_fileunlock(&file_id, &status_func, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in unlocking the file. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    if (status_func != 0) {
        char error[100];
        snprintf(error, 100, "This file wasn't locked.");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();

    }

    return env.Null();
}

Napi::Value Universe::Format(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string record_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(record_string.c_str());
    const char *record = param.c_str();
    long record_len = strlen(record);

    std::string format_string = info[1].ToString().Utf8Value();
    std::string format_param = UTF8toISO8859_1(format_string.c_str());
    const char *format_c = format_param.c_str();
    long format_len = strlen(format_c);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long status_func;
    
    do {
        ic_fmt((char*)format_c, &format_len, (char*)record, &record_len, buffer, &max_buffer_size, &buffer_len, &status_func);

        if (status_func == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (status_func == IE_BTS);

    if (status_func != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in formatting. Code: %ld", status_func);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::FormList(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string list_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(list_string.c_str());
    const char *list_c = param.c_str();
    long list_len = strlen(list_c);

    long list_number = 0;
    if (info.Length() > 1) {
        list_number = info[1].As<Napi::Number>().Uint32Value();
    }

    long code;
    ic_formlist((char*)list_c, &list_len, &list_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in forming list. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    return env.Null();
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
    this->_session_id = 0;
}

Napi::Function Universe::GetClass(Napi::Env env) {
    return DefineClass(env, "Universe", {
            Universe::InstanceMethod("CallSubroutine", &Universe::CallSubroutine),
            Universe::InstanceMethod("Data", &Universe::Data),
            Universe::InstanceMethod("ClearData", &Universe::ClearData),

            Universe::InstanceMethod("StartSession", &Universe::StartSession),
            Universe::InstanceMethod("EndSession", &Universe::EndSession),

            Universe::InstanceMethod("Open", &Universe::Open),
            Universe::InstanceMethod("Close", &Universe::Close),
            Universe::InstanceMethod("ClearFile", &Universe::ClearFile),

            Universe::InstanceMethod("Select", &Universe::Select),
            Universe::InstanceMethod("ClearSelect", &Universe::ClearSelect),
            Universe::InstanceMethod("ReadNext", &Universe::ReadNext),
            Universe::InstanceMethod("FormList", &Universe::FormList),

            Universe::InstanceMethod("Read", &Universe::Read),
            Universe::InstanceMethod("Write", &Universe::Write),
            Universe::InstanceMethod("Delete", &Universe::Delete),

            Universe::InstanceMethod("Date", &Universe::Date),
            Universe::InstanceMethod("IsAlpha", &Universe::IsAlpha),
            Universe::InstanceMethod("Format", &Universe::Format),
            Universe::InstanceMethod("Extract", &Universe::Extract),

            Universe::InstanceMethod("Execute", &Universe::Execute),
            Universe::InstanceMethod("ContinueExecution", &Universe::ContinueExecution),

            Universe::InstanceMethod("FileInfo", &Universe::FileInfo),
            Universe::InstanceMethod("FileLock", &Universe::FileLock),
            Universe::InstanceMethod("FileUnlock", &Universe::FileUnlock),
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "Universe");
    exports.Set(name, Universe::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
