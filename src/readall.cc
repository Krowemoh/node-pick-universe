#include <stdlib.h>
#include <string.h>

#include "intcall.h"
#include "convert.h"
#include "universe.h"

#include <map>
extern std::map<int, std::string> error_map;

Napi::Value Universe::ReadAll(const Napi::CallbackInfo& info) {

    setlocale(LC_ALL, "en_US.iso88591");

    Napi::Env env = info.Env();

    if (this->_session_id == 0) {
        Napi::TypeError::New(env, "Session has not been started.").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    long file_id = info[0].As<Napi::Number>().Uint32Value();

    long list_number = 0;
    if(info.Length() > 1) {
        list_number = info[1].As<Napi::Number>().Uint32Value();
    }

    long buffer_len;
    long code;
    long count;

    long max_buffer_size = 300 * 100 * 1000;
    char *buffer = (char*)malloc(max_buffer_size * sizeof(char));

    ic_readlist(&list_number, buffer, &max_buffer_size, &buffer_len, &count, &code);

    long lock = IK_READ;
    long status_func;

    Napi::Array records = Napi::Array::New(env, count);

    char *token = strtok(buffer, "\xFE");
    int i = 0;

    while (token != NULL) {
        std::string raw_record_id = token;
        token = strtok(NULL, "\xFE");

        std::string record_id = UTF8toISO8859_1(raw_record_id.c_str());
        long id_len = record_id.length();

        long max_rec_size = 500;

        char* record = (char*)malloc(max_rec_size * sizeof(char));
        long record_len = 0;

        do {
            ic_read(&file_id, &lock, record_id.data(), &id_len, record, &max_rec_size, &record_len, &status_func, &code);

            if (code == IE_BTS) {
                free(record);
                max_rec_size = max_rec_size * 2;
                record = (char*)malloc(max_rec_size * sizeof(char));

            } else if (status_func != 0) {
                free(record);
                std::string error = "Record is locked. Record (" + record_id + ")";
                Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
                return env.Null();

            } else if (record == NULL && record_len == 0) {
                std::string error = "Record does not exist. Record (" + record_id + ")";
                Napi::TypeError::New(env, error).ThrowAsJavaScriptException();
                return env.Null();
            }
        } while (code == IE_BTS);

        if (code != 0) {
            free(record);
            std::string error = "Error in reading record: " + record_id + ". ";
            error += "Code (" + std::to_string(code) + ")  - " + error_map[code];

            Napi::TypeError::New(env, error).ThrowAsJavaScriptException();

            Napi::TypeError::New(env, "Failed to read record").ThrowAsJavaScriptException();
            return env.Null();
        }

        unsigned char * out = iso_8859_1_to_utf8((unsigned char*)record, record_len);
        free(record);
        Napi::String str = Napi::String::New(env, (char*)out);
        free(out);
        records[i] = str;
        i++;
    }

    return records;
}
