#pragma once

#include <napi.h>

class Universe : public Napi::ObjectWrap<Universe>
{
    public:
        Universe(const Napi::CallbackInfo&);
        Napi::Value StartSession(const Napi::CallbackInfo&);
        Napi::Value SetSession(const Napi::CallbackInfo&);
        Napi::Value EndSession(const Napi::CallbackInfo&);
        Napi::Value EndAllSessions(const Napi::CallbackInfo&);

        Napi::Value CallSubroutine(const Napi::CallbackInfo&);
        Napi::Value Data(const Napi::CallbackInfo&);
        Napi::Value ClearData(const Napi::CallbackInfo&);

        Napi::Value Open(const Napi::CallbackInfo&);
        Napi::Value Close(const Napi::CallbackInfo&);
        Napi::Value ClearFile(const Napi::CallbackInfo&);

        Napi::Value Indices(const Napi::CallbackInfo&);

        Napi::Value Select(const Napi::CallbackInfo&);
        Napi::Value SelectIndex(const Napi::CallbackInfo&);
        Napi::Value ClearSelect(const Napi::CallbackInfo&);
        Napi::Value ReadNext(const Napi::CallbackInfo&);
        Napi::Value FormList(const Napi::CallbackInfo&);
        Napi::Value GetList(const Napi::CallbackInfo&);
        Napi::Value ReadList(const Napi::CallbackInfo&);
        
        Napi::Value Read(const Napi::CallbackInfo&);
        Napi::Value ReadValue(const Napi::CallbackInfo&);
        Napi::Value Trans(const Napi::CallbackInfo&);
        Napi::Value Write(const Napi::CallbackInfo&);
        Napi::Value WriteValue(const Napi::CallbackInfo&);
        Napi::Value Delete(const Napi::CallbackInfo&);
        Napi::Value DeleteField(const Napi::CallbackInfo&);

        Napi::Value Date(const Napi::CallbackInfo&);
        Napi::Value Time(const Napi::CallbackInfo&);
        Napi::Value TimeDate(const Napi::CallbackInfo&);

        Napi::Value Lock(const Napi::CallbackInfo&);
        Napi::Value Unlock(const Napi::CallbackInfo&);

        Napi::Value IsAlpha(const Napi::CallbackInfo&);

        Napi::Value Lower(const Napi::CallbackInfo&);
        Napi::Value Raise(const Napi::CallbackInfo&);

        Napi::Value Format(const Napi::CallbackInfo&);
        Napi::Value Extract(const Napi::CallbackInfo&);
        Napi::Value Insert(const Napi::CallbackInfo&);
        Napi::Value Locate(const Napi::CallbackInfo&);

        Napi::Value Execute(const Napi::CallbackInfo&);
        Napi::Value ContinueExecution(const Napi::CallbackInfo&);
        Napi::Value RunIType(const Napi::CallbackInfo&);

        Napi::Value FileInfo(const Napi::CallbackInfo&);
        Napi::Value FileLock(const Napi::CallbackInfo&);
        Napi::Value FileUnlock(const Napi::CallbackInfo&);
        Napi::Value GetLocale(const Napi::CallbackInfo&);
        Napi::Value GetValue(const Napi::CallbackInfo&);
        Napi::Value SetValue(const Napi::CallbackInfo&);

        Napi::Value ICONV(const Napi::CallbackInfo&);
        Napi::Value OCONV(const Napi::CallbackInfo&);

        Napi::Value Remove(const Napi::CallbackInfo&);
        Napi::Value Replace(const Napi::CallbackInfo&);

        Napi::Value RecordLock(const Napi::CallbackInfo&);
        Napi::Value RecordLocked(const Napi::CallbackInfo&);
        Napi::Value Release(const Napi::CallbackInfo&);

        Napi::Value SetTimeout(const Napi::CallbackInfo&);
        Napi::Value SessionInfo(const Napi::CallbackInfo&);

        static Napi::Function GetClass(Napi::Env);

    private:
        std::string _host;
        std::string _username;
        std::string _password;
        std::string _account;
        long _session_id;
};
