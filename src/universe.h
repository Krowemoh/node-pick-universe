#pragma once

#include <napi.h>

class Universe : public Napi::ObjectWrap<Universe>
{
    public:
        Universe(const Napi::CallbackInfo&);
        Napi::Value StartSession(const Napi::CallbackInfo&);
        Napi::Value EndSession(const Napi::CallbackInfo&);

        Napi::Value CallSubroutine(const Napi::CallbackInfo&);

        Napi::Value Read(const Napi::CallbackInfo&);

        static Napi::Function GetClass(Napi::Env);

    private:
        std::string _host;
        std::string _username;
        std::string _password;
        std::string _account;
};
