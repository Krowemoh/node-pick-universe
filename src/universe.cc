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

    if (code != 0) {
        free(record);
        char error[500];
        snprintf(error, 500, "Failed to read record.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record, record_len);
    free(record);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::ReadValue(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long field_number = info[1].As<Napi::Number>().Uint32Value();

    long file_id = info[2].As<Napi::Number>().Uint32Value();

    long max_field_size = 500;
    char* field = (char*)malloc(max_field_size * sizeof(char));
    long field_len = 0;

    long lock = IK_READ;
    long status_func;
    long code;
    do {
        ic_readv(&file_id, &lock, (char*)record_id, &id_len, &field_number, field, &max_field_size, &field_len, &status_func, &code);
        if (code == IE_BTS) {
            free(field);
            max_field_size = max_field_size * 2;
            field = (char*)malloc(max_field_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(field);
        char error[500];
        snprintf(error, 500, "Record does not exist.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)field, field_len);
    free(field);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::Trans(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    std::string filename_string = info[1].ToString().Utf8Value();
    const char *filename = filename_string.c_str();
    long file_len = strlen(filename);

    long field_pos = -1;
    if (info.Length() > 2) {
        field_pos = info[2].As<Napi::Number>().Uint32Value();
    }

    long dict_flag = IK_DATA;

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    const char *control_code = "X";
    long control_code_len = strlen(control_code);

    long status_func;
    long code;

    do {
        ic_trans((char*)filename, &file_len, &dict_flag, (char*)record_id, &id_len, &field_pos, (char *)control_code, &control_code_len, buffer, &max_buffer_size, &buffer_len, &status_func, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[500];
        snprintf(error, 500, "Error in reading record. Code (%ld), Status (%ld).\n", code, status_func);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

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

Napi::Value WriteValueBase(const Napi::CallbackInfo& info, long lock_type) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();


    std::string field_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(field_string.c_str());
    const char *field = param.c_str();
    long field_len = strlen(field);

    std::string record_id_string = info[1].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long field_number = info[2].As<Napi::Number>().Uint32Value();

    long file_id = info[3].As<Napi::Number>().Uint32Value();

    long status_func;
    long code;

    ic_writev(&file_id, &lock_type, (char*)record_id, &id_len, &field_number, (char*)field, &field_len, &status_func, &code);

    if (code == IE_LCK) {
        char error[100];
        snprintf(error, 100, "Record is locked. Record: %s\n", record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    } else if (code != 0) {
        char error[200];
        snprintf(error, 200, "Error in writing value record. Code (%ld), Status (%ld). Record: %s\n", code, status_func, record_id);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return info.Env().Null();
}

Napi::Value Universe::WriteValue(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_type = IK_WRITE;
    return WriteValueBase(info, lock_type);
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

Napi::Value Universe::Indices(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();

    std::string index_string = "";
    if (info.Length() > 1) {
        index_string = info[1].ToString().Utf8Value();
    }

    const char* ak_name = index_string.c_str();
    long ak_len = strlen(ak_name);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;
    do {
        ic_indices(&file_id, (char*)ak_name, &ak_len, buffer, &max_buffer_size, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Failed to get indices. Code = %ld.\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
 
    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::SelectIndex(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string index_string = info[0].ToString().Utf8Value();
    const char* ak_name = index_string.c_str();
    long ak_len = strlen(ak_name);

    std::string value_string = info[1].ToString().Utf8Value();
    const char* ak_value = value_string.c_str();
    long ak_value_len = strlen(ak_value);

    long file_id = info[2].As<Napi::Number>().Uint32Value();

    long list_number = 0;
    if(info.Length() > 3) {
        list_number = info[3].As<Napi::Number>().Uint32Value();
    }

    long status_func;
    long code;
    ic_selectindex(&file_id, &list_number, (char*)ak_name, &ak_len, (char*)ak_value, &ak_value_len, &status_func, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "SelectIndex failed. Code = %ld.\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    if (status_func != 0) {
        char error[100];
        snprintf(error, 100, "SelectIndex failed. Status = %ld.\n", status_func);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
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

Napi::Value Universe::ReadList(const Napi::CallbackInfo& info) {
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

    long max_buffer_size = 500;
    char *buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len;

    long code;
    long count;

    do {
        ic_readlist(&list_number, buffer, &max_buffer_size, &buffer_len, &count, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Readlist failed. Code = %ld.\n", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
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

Napi::Value Universe::SetSession(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long session_id = info[0].As<Napi::Number>().Uint32Value();

    long code;
    ic_setsession(&session_id, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to set session. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    this->_session_id = session_id;
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

Napi::Value Universe::EndAllSessions(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_quitall(&code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to close sessions. Code = %ld\n", code);
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

Napi::Value Universe::Time(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long time;
    long code;
    ic_time(&time, &code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to get time. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Env env = info.Env();
    return Napi::Number::New(env, time);
}

Napi::Value Universe::TimeDate(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long max_buffer_size = 21;
    char buffer[max_buffer_size];
    long buffer_len;

    long code;
    ic_timedate(buffer, &max_buffer_size, &buffer_len, &code);
    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to get time. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    Napi::Env env = info.Env();
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
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

Napi::Value Universe::Lower(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string temp = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(temp.c_str());

    const char *buffer = param.c_str();
    long buffer_len = strlen(buffer);

    long code;
    ic_lower((char *)buffer, &buffer_len, &code);

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    Napi::Env env = info.Env();
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);

    return data;
}

Napi::Value Universe::Raise(const Napi::CallbackInfo& info) {
    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string temp = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(temp.c_str());

    const char *buffer = param.c_str();
    long buffer_len = strlen(buffer);

    long code;
    ic_raise((char *)buffer, &buffer_len, &code);

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    Napi::Env env = info.Env();
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);

    return data;
}

Napi::Value Universe::Lock(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_number = info[0].As<Napi::Number>().Uint32Value();
    if (lock_number < 0 || lock_number > 63) {
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, "Invalid lock.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_lock(&lock_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to get lock. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Value Universe::Unlock(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long lock_number = info[0].As<Napi::Number>().Uint32Value();
    if (lock_number < 0 || lock_number > 63) {
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, "Invalid lock.").ThrowAsJavaScriptException();
        return env.Null();
    }

    long code;
    ic_unlock(&lock_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Failed to unlock. Code = %ld\n", code);
        Napi::Env env = info.Env();
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
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

Napi::Value Universe::RunIType(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long record_id_len = strlen(record_id);

    std::string filename_string = info[1].ToString().Utf8Value();
    const char *filename = filename_string.c_str();
    long filename_len = strlen(filename);

    std::string itype_string = info[2].ToString().Utf8Value();
    const char *itype = itype_string.c_str();
    long itype_len = strlen(itype);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;

    do {
        ic_itype((char*)filename, &filename_len, (char*)record_id, &record_id_len, (char*)itype, &itype_len, buffer, &max_buffer_size, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in getting session info. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
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

Napi::Value Universe::GetList(const Napi::CallbackInfo& info) {
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
    ic_getlist((char*)list_c, &list_len, &list_number, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in getting list. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }
    return env.Null();
}

Napi::Value Universe::GetLocale(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long key = IK_LC_ALL;
    if (info.Length() > 0) {
        key = info[0].As<Napi::Number>().Uint32Value();
    }

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;
    do {
        ic_get_locale(&key, buffer, &max_buffer_size, &buffer_len, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in getting locale. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::GetValue(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long key = 0;
    if (info.Length() > 0) {
        key = info[0].As<Napi::Number>().Uint32Value();
    }

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;
    do {
        ic_getvalue(&key, buffer, &max_buffer_size, &buffer_len, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in getting value. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::SetValue(const Napi::CallbackInfo& info) {
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

    long key = 0;
    if (info.Length() > 1) {
        key = info[1].As<Napi::Number>().Uint32Value();
    }

    long code;
    ic_setvalue(&key, (char*)record, &record_len, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in setting value. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Value Universe::ICONV(const Napi::CallbackInfo& info) {
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

    std::string conversion_string = info[1].ToString().Utf8Value();
    std::string conversion_param = UTF8toISO8859_1(conversion_string.c_str());
    const char *conversion = conversion_param.c_str();
    long conversion_len = strlen(conversion);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;
    do {
        ic_iconv((char*)conversion, &conversion_len, (char*)record, &record_len, buffer, &max_buffer_size, &buffer_len, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0 && code != 1 && code != 2 && code != 3) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in iconv. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::OCONV(const Napi::CallbackInfo& info) {
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

    std::string conversion_string = info[1].ToString().Utf8Value();
    std::string conversion_param = UTF8toISO8859_1(conversion_string.c_str());
    const char *conversion = conversion_param.c_str();
    long conversion_len = strlen(conversion);

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;
    do {
        ic_oconv((char*)conversion, &conversion_len, (char*)record, &record_len, buffer, &max_buffer_size, &buffer_len, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0 && code != 1 && code != 2 && code != 3) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in iconv. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::RecordLock(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long lock = IK_READL;
    if (info.Length() > 2) {
       lock = info[2].As<Napi::Number>().Uint32Value();
    }

    long status_func;
    long code;

    ic_recordlock(&file_id, &lock, (char*)record_id, &id_len, &status_func, &code);

    if (code != 0) {
        char error[500];
        snprintf(error, 500, "Failed to get a recordlock on the record.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Number data = Napi::Number::New(env, status_func);
    return data;
}

Napi::Value Universe::RecordLocked(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long lock_status;

    long status_func;
    long code;

    ic_recordlocked(&file_id, (char*)record_id, &id_len, &lock_status, &status_func, &code);

    if (code != 0) {
        char error[500];
        snprintf(error, 500, "Failed to get a recordlock on the record.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Number data = Napi::Number::New(env, lock_status);
    return data;
}

Napi::Value Universe::Release(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string record_id_string = info[0].ToString().Utf8Value();
    const char *record_id = record_id_string.c_str();
    long id_len = strlen(record_id);

    long file_id = info[1].As<Napi::Number>().Uint32Value();

    long code;
    ic_release(&file_id, (char*)record_id, &id_len, &code);

    if (code != 0) {
        char error[500];
        snprintf(error, 500, "Failed to get a recordlock on the record.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Value Universe::Remove(const Napi::CallbackInfo& info) {
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

    long delimiter = info[1].As<Napi::Number>().Uint32Value();

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long remove_ptr = 0;
    long code;
    do {
        ic_remove((char*)record, &record_len, buffer, &max_buffer_size, &buffer_len, &delimiter, &remove_ptr, &code);
        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in removing field. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::Insert(const Napi::CallbackInfo& info) {
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

    long max_buffer_size = record_len*2;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = record_len;
    memcpy(buffer, record, record_len);

    std::string insert_string = info[1].ToString().Utf8Value();
    std::string insert_param = UTF8toISO8859_1(insert_string.c_str());
    const char *insert = insert_param.c_str();
    long insert_len = strlen(insert);

    long field_pos = 0;
    if (info.Length() > 2) {
        field_pos = info[2].As<Napi::Number>().Uint32Value();
    }

    long value_pos = 0;
    if (info.Length() > 3) {
        value_pos = info[3].As<Napi::Number>().Uint32Value();
    }

    long subvalue_pos = 0;
    if (info.Length() > 4) {
        subvalue_pos = info[4].As<Napi::Number>().Uint32Value();
    }

    long code;

    do {
        ic_insert((char*)buffer, &max_buffer_size, &buffer_len, &field_pos, &value_pos, &subvalue_pos, (char*)insert, &insert_len, &code);
        if (code == IE_BTS) {
            max_buffer_size = max_buffer_size * 2;
            free(buffer);
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
            memcpy(buffer, record, record_len);
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in insertion. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::DeleteField(const Napi::CallbackInfo& info) {
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

    long field_pos = 0;
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
    ic_strdel((char*)record, &record_len, &field_pos, &value_pos, & subvalue_pos, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in removing field. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record, record_len);
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::Replace(const Napi::CallbackInfo& info) {
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

    long max_buffer_size = record_len*2;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = record_len;
    memcpy(buffer, record, record_len);

    std::string replace_string = info[1].ToString().Utf8Value();
    std::string replace_param = UTF8toISO8859_1(replace_string.c_str());
    const char *replace = replace_param.c_str();
    long replace_len = strlen(replace);

    long field_pos = 0;
    if (info.Length() > 2) {
        field_pos = info[2].As<Napi::Number>().Uint32Value();
    }

    long value_pos = 0;
    if (info.Length() > 3) {
        value_pos = info[3].As<Napi::Number>().Uint32Value();
    }

    long subvalue_pos = 0;
    if (info.Length() > 4) {
        subvalue_pos = info[4].As<Napi::Number>().Uint32Value();
    }

    long code;
    do {
        ic_replace((char*)buffer, &max_buffer_size, &buffer_len, &field_pos, &value_pos, & subvalue_pos, (char*)replace, &replace_len, &code);
        if (code == IE_BTS) {
            max_buffer_size = max_buffer_size * 2;
            free(buffer);
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
            memcpy(buffer, record, record_len);
        }
    } while (code == IE_BTS);

    if (code != 0) {
        free(buffer);
        char error[100];
        snprintf(error, 100, "Error in removing field. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);
    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
}

Napi::Value Universe::SetTimeout(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::Env env = info.Env();
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long timeout = info[0].As<Napi::Number>().Uint32Value();

    long code;
    ic_set_comms_timeout(&timeout, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in setting timeout. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Value Universe::Locate(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string search_string = info[0].ToString().Utf8Value();
    std::string param = UTF8toISO8859_1(search_string.c_str());
    const char *search = param.c_str();
    long search_len = strlen(search);

    std::string haystack_string = info[1].ToString().Utf8Value();
    std::string haystack_param = UTF8toISO8859_1(haystack_string.c_str());
    const char *haystack = haystack_param.c_str();
    long haystack_len = strlen(haystack);

    std::string order_string = info[2].ToString().Utf8Value();
    std::string order_param = UTF8toISO8859_1(order_string.c_str());
    const char *order = order_param.c_str();
    long order_len = strlen(order);

    long start = 1;
    if (info.Length() > 3) {
        start = info[3].As<Napi::Number>().Uint32Value();
    }

    long field_pos = 0;
    if (info.Length() > 4) {
        field_pos = info[4].As<Napi::Number>().Uint32Value();
    }

    long value_pos = 0;
    if (info.Length() > 5) {
        value_pos = info[5].As<Napi::Number>().Uint32Value();
    }

    long index;
    long found;
    long code;

    ic_locate((char*)search, &search_len, (char*)haystack, &haystack_len, &field_pos, &value_pos, &start, (char*)order, &order_len, &index, &found, &code);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in locate. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Array arguments = Napi::Array::New(env, 2);
    arguments[(int)0] = found;
    arguments[1] = index;
    return arguments;
}

Napi::Value Universe::SessionInfo(const Napi::CallbackInfo& info) {
    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        char error[100];
        snprintf(error, 100, "Session has not been started.\n");
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    long key = info[0].As<Napi::Number>().Uint32Value();

    long max_buffer_size = 500;
    char* buffer = (char*)malloc(max_buffer_size * sizeof(char));
    long buffer_len = 0;

    long code;

    do {
        ic_session_info(&key, buffer, &max_buffer_size, &buffer_len, &code);

        if (code == IE_BTS) {
            free(buffer);
            max_buffer_size = max_buffer_size * 2;
            buffer = (char*)malloc(max_buffer_size * sizeof(char));
        }
    } while (code == IE_BTS);

    if (code != 0) {
        char error[100];
        snprintf(error, 100, "Error in getting session info. Code: %ld", code);
        Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
        return env.Null();
    }

    unsigned char * out = iso_8859_1_to_utf8((unsigned char*)buffer, buffer_len);
    free(buffer);

    Napi::String data = Napi::String::New(env, (char*)out);
    free(out);
    return data;
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
            Universe::InstanceMethod("SetSession", &Universe::SetSession),
            Universe::InstanceMethod("EndSession", &Universe::EndSession),
            Universe::InstanceMethod("EndAllSessions", &Universe::EndAllSessions),

            Universe::InstanceMethod("Open", &Universe::Open),
            Universe::InstanceMethod("Close", &Universe::Close),
            Universe::InstanceMethod("ClearFile", &Universe::ClearFile),

            Universe::InstanceMethod("Indices", &Universe::Indices),

            Universe::InstanceMethod("Select", &Universe::Select),
            Universe::InstanceMethod("SelectIndex", &Universe::SelectIndex),
            Universe::InstanceMethod("ClearSelect", &Universe::ClearSelect),
            Universe::InstanceMethod("ReadNext", &Universe::ReadNext),
            Universe::InstanceMethod("FormList", &Universe::FormList),
            Universe::InstanceMethod("GetList", &Universe::GetList),
            Universe::InstanceMethod("ReadList", &Universe::ReadList),

            Universe::InstanceMethod("Read", &Universe::Read),
            Universe::InstanceMethod("ReadValue", &Universe::ReadValue),
            Universe::InstanceMethod("Trans", &Universe::Trans),
            Universe::InstanceMethod("Write", &Universe::Write),
            Universe::InstanceMethod("WriteValue", &Universe::WriteValue),
            Universe::InstanceMethod("Delete", &Universe::Delete),
            Universe::InstanceMethod("DeleteField", &Universe::DeleteField),

            Universe::InstanceMethod("Date", &Universe::Date),
            Universe::InstanceMethod("Time", &Universe::Time),
            Universe::InstanceMethod("TimeDate", &Universe::TimeDate),

            Universe::InstanceMethod("Lock", &Universe::Lock),
            Universe::InstanceMethod("Unlock", &Universe::Unlock),

            Universe::InstanceMethod("IsAlpha", &Universe::IsAlpha),

            Universe::InstanceMethod("Lower", &Universe::Lower),
            Universe::InstanceMethod("Raise", &Universe::Raise),

            Universe::InstanceMethod("Format", &Universe::Format),
            Universe::InstanceMethod("Extract", &Universe::Extract),
            Universe::InstanceMethod("Insert", &Universe::Insert),
            Universe::InstanceMethod("Locate", &Universe::Locate),

            Universe::InstanceMethod("Execute", &Universe::Execute),
            Universe::InstanceMethod("ContinueExecution", &Universe::ContinueExecution),
            Universe::InstanceMethod("RunIType", &Universe::RunIType),

            Universe::InstanceMethod("FileInfo", &Universe::FileInfo),
            Universe::InstanceMethod("FileLock", &Universe::FileLock),
            Universe::InstanceMethod("FileUnlock", &Universe::FileUnlock),
            Universe::InstanceMethod("GetLocale", &Universe::GetLocale),
            Universe::InstanceMethod("GetValue", &Universe::GetValue),
            Universe::InstanceMethod("SetValue", &Universe::SetValue),

            Universe::InstanceMethod("ICONV", &Universe::ICONV),
            Universe::InstanceMethod("OCONV", &Universe::OCONV),

            Universe::InstanceMethod("Remove", &Universe::Remove),
            Universe::InstanceMethod("Replace", &Universe::Replace),

            Universe::InstanceMethod("RecordLock", &Universe::RecordLock),
            Universe::InstanceMethod("RecordLocked", &Universe::RecordLocked),
            Universe::InstanceMethod("Release", &Universe::Release),

            Universe::InstanceMethod("SetTimeout", &Universe::SetTimeout),
            Universe::InstanceMethod("SessionInfo", &Universe::SessionInfo),
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "Universe");
    exports.Set(name, Universe::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
