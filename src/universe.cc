#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "intcall.h"
#include <string.h>
#include <ctype.h>
#include <locale.h>

#include "convert.h"
#include "universe.h"

using namespace Napi;

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
