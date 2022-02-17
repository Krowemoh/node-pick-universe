#pragma once

#include <napi.h>

class Universe : public Napi::ObjectWrap<Universe>
{
    public:
        Universe(const Napi::CallbackInfo&);
        Napi::Value StartSession(const Napi::CallbackInfo&);
        Napi::Value EndSession(const Napi::CallbackInfo&);

        Napi::Value CallSubroutine(const Napi::CallbackInfo&);
        Napi::Value Data(const Napi::CallbackInfo&);
        Napi::Value ClearData(const Napi::CallbackInfo&);

        Napi::Value Open(const Napi::CallbackInfo&);
        Napi::Value Close(const Napi::CallbackInfo&);
        Napi::Value ClearFile(const Napi::CallbackInfo&);

        Napi::Value Select(const Napi::CallbackInfo&);
        Napi::Value ClearSelect(const Napi::CallbackInfo&);
        Napi::Value ReadNext(const Napi::CallbackInfo&);
        Napi::Value FormList(const Napi::CallbackInfo&);
        
        Napi::Value Read(const Napi::CallbackInfo&);
        Napi::Value Write(const Napi::CallbackInfo&);
        Napi::Value Delete(const Napi::CallbackInfo&);

        Napi::Value Date(const Napi::CallbackInfo&);
        Napi::Value IsAlpha(const Napi::CallbackInfo&);
        Napi::Value Format(const Napi::CallbackInfo&);
        Napi::Value Extract(const Napi::CallbackInfo&);

        Napi::Value Execute(const Napi::CallbackInfo&);
        Napi::Value ContinueExecution(const Napi::CallbackInfo&);

        Napi::Value FileInfo(const Napi::CallbackInfo&);
        Napi::Value FileLock(const Napi::CallbackInfo&);
        Napi::Value FileUnlock(const Napi::CallbackInfo&);

        static Napi::Function GetClass(Napi::Env);

    private:
        std::string _host;
        std::string _username;
        std::string _password;
        std::string _account;
        long _session_id;
};
