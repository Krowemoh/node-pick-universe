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
            Universe::InstanceMethod("ClearData", &Universe::ClearData),
            Universe::InstanceMethod("ClearFile", &Universe::ClearFile),
            Universe::InstanceMethod("ClearSelect", &Universe::ClearSelect),
            Universe::InstanceMethod("Close", &Universe::Close),
            Universe::InstanceMethod("ContinueExecution", &Universe::ContinueExecution),

            Universe::InstanceMethod("Data", &Universe::Data),
            Universe::InstanceMethod("Date", &Universe::Date),
            Universe::InstanceMethod("Delete", &Universe::Delete),
            Universe::InstanceMethod("DeleteField", &Universe::DeleteField),

            Universe::InstanceMethod("EndAllSessions", &Universe::EndAllSessions),
            Universe::InstanceMethod("EndSession", &Universe::EndSession),
            Universe::InstanceMethod("Execute", &Universe::Execute),
            Universe::InstanceMethod("Extract", &Universe::Extract),
        
            Universe::InstanceMethod("FileInfo", &Universe::FileInfo),
            Universe::InstanceMethod("FileLock", &Universe::FileLock),
            Universe::InstanceMethod("FileUnlock", &Universe::FileUnlock),
            Universe::InstanceMethod("Format", &Universe::Format),
            Universe::InstanceMethod("FormList", &Universe::FormList),

            Universe::InstanceMethod("GetList", &Universe::GetList),
            Universe::InstanceMethod("GetLocale", &Universe::GetLocale),
            Universe::InstanceMethod("GetValue", &Universe::GetValue),
            
            Universe::InstanceMethod("ICONV", &Universe::ICONV),
            Universe::InstanceMethod("Indices", &Universe::Indices),
            Universe::InstanceMethod("Insert", &Universe::Insert),
            Universe::InstanceMethod("IsAlpha", &Universe::IsAlpha),

            Universe::InstanceMethod("Locate", &Universe::Locate),
            Universe::InstanceMethod("Lock", &Universe::Lock),
            Universe::InstanceMethod("Lower", &Universe::Lower),

            Universe::InstanceMethod("OCONV", &Universe::OCONV),
            Universe::InstanceMethod("Open", &Universe::Open),
            
            Universe::InstanceMethod("Raise", &Universe::Raise),
            Universe::InstanceMethod("Read", &Universe::Read),
            Universe::InstanceMethod("ReadList", &Universe::ReadList),
            Universe::InstanceMethod("ReadNext", &Universe::ReadNext),
            Universe::InstanceMethod("ReadValue", &Universe::ReadValue),
            Universe::InstanceMethod("RecordLock", &Universe::RecordLock),
            Universe::InstanceMethod("RecordLocked", &Universe::RecordLocked),
            Universe::InstanceMethod("Release", &Universe::Release),
            Universe::InstanceMethod("Remove", &Universe::Remove),
            Universe::InstanceMethod("Replace", &Universe::Replace),
            Universe::InstanceMethod("RunIType", &Universe::RunIType),

            Universe::InstanceMethod("Select", &Universe::Select),
            Universe::InstanceMethod("SelectIndex", &Universe::SelectIndex),
            Universe::InstanceMethod("SessionInfo", &Universe::SessionInfo),
            Universe::InstanceMethod("SetSession", &Universe::SetSession),
            Universe::InstanceMethod("SetTimeout", &Universe::SetTimeout),
            Universe::InstanceMethod("SetValue", &Universe::SetValue),
            Universe::InstanceMethod("StartSession", &Universe::StartSession),

            Universe::InstanceMethod("Time", &Universe::Time),
            Universe::InstanceMethod("TimeDate", &Universe::TimeDate),
            Universe::InstanceMethod("Trans", &Universe::Trans),

            Universe::InstanceMethod("Unlock", &Universe::Unlock),

            Universe::InstanceMethod("Write", &Universe::Write),
            Universe::InstanceMethod("WriteValue", &Universe::WriteValue),          
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "Universe");
    exports.Set(name, Universe::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
