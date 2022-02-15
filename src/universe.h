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
        Napi::Value Read(const Napi::CallbackInfo&);
        Napi::Value Write(const Napi::CallbackInfo&);

        static Napi::Function GetClass(Napi::Env);

    private:
        std::string _host;
        std::string _username;
        std::string _password;
        std::string _account;
        long _session_id;
};
