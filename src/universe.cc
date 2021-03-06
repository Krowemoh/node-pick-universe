#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "intcall.h"
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <map>

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
            std::string error = "Error, argument is not a string. Position (" + std::to_string(i) + ")";
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
            Universe::InstanceMethod("ReadAll", &Universe::ReadAll),
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

/* Errors */

std::map<int, std::string> error_map = {
    { 14000, "First error number in range" }, 
    { 14002, "No such file or directory" }, 
    { 14005, "I/O error" }, 
    { 14009, "Bad file number" }, 
    { 14012, "No memory available" }, 
    { 14013, "Permission denied" }, 
    { 14022, "Invalid argument" }, 
    { 14023, "File table overflow" }, 
    { 14024, "Too many open files" }, 
    { 14028, "No space left on device" }, 
    { 14200, "Beginning of range" }, 
    { 14500, "Offset for error mapping" }, 
    { 14495, "VSL network type not specified" }, 
    { 14499, "VSL network module not" }, 
    { 14535, "Operation would block" }, 
    { 14536, "Operation now in progress" }, 
    { 14537, "Operation already in" }, 
    { 14538, "Socket operation on" }, 
    { 14539, "Destination address required" }, 
    { 14540, "Message too long" }, 
    { 14541, "Protocol wrong type for" }, 
    { 14542, "Bad protocol option" }, 
    { 14543, "Protocol not supported" }, 
    { 14544, "Socket type not supported" }, 
    { 14545, "Operation not supported on" }, 
    { 14546, "Protocol family not" }, 
    { 14547, "Addr family not supported by prot" }, 
    { 14548, "Address already in use" }, 
    { 14549, "Can't assign requested" }, 
    { 14550, "Network is down" }, 
    { 14551, "Network is unreachable" }, 
    { 14552, "Network dropped connection or" }, 
    { 14553, "Software caused connection" }, 
    { 14554, "Connection reset by peer" }, 
    { 14555, "No buffer space available" }, 
    { 14556, "Socket is already connected" }, 
    { 14557, "Socket is not connected" }, 
    { 14558, "Can't send after socket" }, 
    { 14559, "Too many references: can't" }, 
    { 14560, "Connection timed out" }, 
    { 14561, "Connection refused" }, 
    { 14562, "Too many levels of symbolic" }, 
    { 14563, "File name too long" }, 
    { 14564, "Host is down" }, 
    { 14565, "Host is unreachable" }, 
    { 14566, "Directory not empty" }, 
    { 14567, "Too many processes" }, 
    { 14568, "Too many users" }, 
    { 14569, "Disc quota exceeded" }, 
    { 14570, "Stale NFS file handle" }, 
    { 14571, "Too many levels of remote in" }, 
    { 14572, "Device is not a stream" }, 
    { 14573, "Timer expired" }, 
    { 14574, "Out of streams resources" }, 
    { 14575, "No message of desired type" }, 
    { 14576, "Trying to read unreadable" }, 
    { 14577, "Identifier removed" }, 
    { 14578, "Deadlock condition." }, 
    { 14579, "No record locks available." }, 
    { 14580, "Library/driver version" }, 
    { 14581, "Invalid argument" }, 
    { 14582, "Too many open sockets" }, 
    { 14583, "Bad address in sockets call" }, 
    { 14584, "The socket has reset" }, 
    { 14585, "Unique parameter required" }, 
    { 14586, "Gateway address required" }, 
    { 14587, "The packet could not be sent" }, 
    { 14588, "No driver or card failed" }, 
    { 14589, "Queued write operation" }, 
    { 14590, "Queued read operation" }, 
    { 14591, "TCPIP not loaded" }, 
    { 14592, "TCPIP busy" }, 
    { 14999, "End of range" }, 
    { 10000, "First PI-specific error" }, 
    { 11000, "Requested access denied" }, 
    { 11001, "Function not supported on this" }, 
    { 11002, "Relative pathname expected and not" }, 
    { 11003, "Pathname could not be found" }, 
    { 11004, "Device not assigned" }, 
    { 11005, "Device not known" }, 
    { 11006, "Device assigned with Read Only" }, 
    { 11007, "Bad stty option when device" }, 
    { 11008, "Attempting to send message to user not in" }, 
    { 11009, "Sender requires receive" }, 
    { 11010, "Message rejected by" }, 
    { 22000, "Already initialised" }, 
    { 22001, "bad field number (READV," }, 
    { 22002, "buffer.size too small or not valid" }, 
    { 22003, "Illegal record ID" }, 
    { 22004, "last record read (READNEXT)" }, 
    { 22005, "file.tag is not a file" }, 
    { 22006, "Client library not initialised for this" }, 
    { 22007, "The file was not locked by your" }, 
    { 22008, "Prime INFORMATION is not" }, 
    { 22009, "The FILEINFO result is a" }, 
    { 22010, "no memory to DIM an array" }, 
    { 22011, "Error releasing memory" }, 
    { 22012, "Bad Partitioned file" }, 
    { 22013, "Bad Partitioning algorithm" }, 
    { 22014, "Non-unique Part number" }, 
    { 22015, "Dynt not available" }, 
    { 30001, "Record not found" }, 
    { 30002, "File or record is locked by another" }, 
    { 30007, "File table (ie smm) full" }, 
    { 30010, "Bad parameter" }, 
    { 30011, "Bad key" }, 
    { 30012, "File already exists in an attempt to" }, 
    { 30013, "Wrong file type: not segdir or" }, 
    { 30014, "File opened exclusively by another" }, 
    { 30015, "Rwlock on file is wrong and can't be" }, 
    { 30016, "Wrong subfile type" }, 
    { 30018, "Subfile not found when" }, 
    { 30019, "Bad header in memory file" }, 
    { 30020, "Unsupported file type" }, 
    { 30021, "Unknown file type detected" }, 
    { 30031, "Illegal operation on file" }, 
    { 30049, "Shared data for file doesn't match" }, 
    { 30052, "Bad header in LH primary" }, 
    { 30053, "Bad header in LH overflow" }, 
    { 30075, "Bad file name" }, 
    { 30086, "Unimplemented FILEINFO" }, 
    { 30094, "Bad ID length" }, 
    { 30095, "Fileid is incorrect for session" }, 
    { 30096, "Unsupport Server command, functions not availble let" }, 
    { 30097, "Select Failed" }, 
    { 30098, "Lock number provided is invalid" }, 
    { 30099, "Filed opened for sequential access and hashed access tried" }, 
    { 30100, "Filed opened for hashed access and sequential access tried" }, 
    { 30101, "Seek command failed" }, 
    { 30102, "Internal datum error" }, 
    { 30103, "Invalid Key used for GET/SET at variables" }, 
    { 30104, "FILEINFO Key out of range" }, 
    { 30105, "Unable to load subroutine on host" }, 
    { 30106, "Bad number of arguments for subroutine, either too many or not enough" }, 
    { 30107, "Subroutine failed to complete suceesfully" }, 
    { 30108, "IType failed to complete correctly" }, 
    { 30109, "IType failed to load" }, 
    { 30110, "The IType has not been compiled" }, 
    { 30111, "It is not an itype or the itype is corrupt" }, 
    { 30112, "Filename is null" }, 
    { 30113, "Weofseq failed" }, 
    { 30114, "An execute is currently active" }, 
    { 30115, "An execute is currently active" }, 
    { 30116, "Internal execute error, execute has not return an expected status" }, 
    { 30117, "Blocksize is invalid for call" }, 
    { 30118, "Bad trans control code" }, 
    { 30119, "Execute did not send returncodes bad to client correctly" }, 
    { 30120, "failure to dup ttys" }, 
    { 30121, "Bad Transaction Key" }, 
    { 30122, "Transaction commit has failed" }, 
    { 30123, "Transaction rollback has failed" }, 
    { 30124, "A Transaction is active so this action is forbidden" }, 
    { 30125, "Can not access part files" }, 
    { 30126, "failed to cancel an execute" }, 
    { 30127, "Bad key for ic_session_info" }, 
    { 30128, "create of sequential file failed" }, 
    { 30129, "Failed to duplicate a pipe handle" }, 
    { 31000, "No VOC record" }, 
    { 31001, "No pathname in VOC record" }, 
    { 31002, "VOC file record not a File" }, 
    { 31100, "Clear file no exclusive" }, 
    { 33200, "Select list not active" }, 
    { 33201, "Bad parameter 1" }, 
    { 33202, "Bad parameter 2" }, 
    { 33203, "Bad parameter 3" }, 
    { 33204, "Bad parameter 4" }, 
    { 33205, "Bad parameter 5" }, 
    { 33206, "Bad parameter 6" }, 
    { 33207, "Bad parameter 7" }, 
    { 33208, "Bad parameter 8" }, 
    { 33209, "Bad parameter 9" }, 
    { 33211, "Bad select list number" }, 
    { 33212, "Bad partfile id" }, 
    { 33213, "Bad AK file" }, 
    { 39000, "command not recognized by" }, 
    { 39001, "no way to perform a LOGOUT" }, 
    { 39002, "data.length not a valid" }, 
    { 39003, "can't open the VOC file" }, 
    { 39004, "internal - client RESET received" }, 
    { 39005, "@SYSTEM.RETURN.CODE non-numeric after" }, 
    { 39006, "@SYSTEM.RETURN.CODE has more than 2" }, 
    { 39007, "interCALL server key not" }, 
    { 39008, "WRITE failed and taken ELSE" }, 
    { 39101, "Host not responding" }, 
    { 39102, "Synchroniser not received" }, 
    { 39103, "Timeout on receving packets" }, 
    { 39104, "Host length error on receive" }, 
    { 39105, "Host does not give ready" }, 
    { 39106, "Packet not acknowledged" }, 
    { 39107, "Too many concurrent user" }, 
    { 39108, "Could not lock user data" }, 
    { 39109, "Library in use" }, 
    { 39110, "Host got incorrect length from" }, 
    { 39111, "Host response non-numeric" }, 
    { 39112, "Host length error on receive" }, 
    { 39113, "No data in host response" }, 
    { 39114, "Host name missing from script" }, 
    { 39115, "Host has closed socket" }, 
    { 39116, "Failed to get address for this" }, 
    { 39117, "Fatal error" }, 
    { 39118, "Bad error number from host, i.e. error" }, 
    { 39119, "Server waiting for input" }, 
    { 39120, "Session is not opened when an action has be tried on it" }, 
    { 39121, "The Universe license has expired" }, 
    { 39122, "Client or server is out of date Client/server functions have been" }, 
    { 39123, "Client or server is out of date comms support has been updated" }, 
    { 39124, "Incorrect client/server being commuincated with" }, 
    { 39125, "The dicteroy you are connecting to, either is not a universe account or does not exist" }, 
    { 39126, "An error has occurred on the server when trying to transmit an error code to the client" }, 
    { 39127, "Unable to get the uv home coorectly" }, 
    { 39128, "Bad path found UV.ACCUNTS file" }, 
    { 39129, "Account name given is not an account" }, 
    { 39130, "UV.ACCOUNT file could not be found to opened" }, 
    { 39131, "Failed to attach to the account specified" }, 
    { 39132, "not a valid universe account" }, 
    { 39133, "failed to setup the terminal for server" }, 
    { 39134, "user limited reached */" }, 
    { 39135, "NLS support not available" }, 
    { 39200, "Server failed to create the slave pipes" }, 
    { 39201, "Server failed to connect to socket" }, 
    { 39202, "Slave failed to give server the Go Ahead message" }, 
    { 39203, "Failed to allocate memory for the message from the slave" }, 
    { 39204, "The slave failed to start correctly" }, 
    { 39205, "Failed to the pass the message to the slave correctly" }, 
    { 39206, "Server failed to allocate the memory for the execute buffer correctly" }, 
    { 39207, "Failed to read from the slave correctly" }, 
    { 39208, "Failed to write the reply to the slave (ic_inputreply)" }, 
    { 39209, "Failed to read the size of the message from the slave" }, 
    { 39210, "Server failed to select() on input channel" }, 
    { 39211, "The select has timed out" }, 
    { 80011, "login name or password provided is incorrect" }, 
    { 80019, "password provided has expired" }, 
    { 80036, "Remote authorisation failed" }, 
    { 80144, "The account has expired" }, 
    { 80147, "Unable to run on the remote machine as the given user." }, 
    { 80148, "Unable to update user details." }, 
    { 81001, "connection is bad" }, 
    { 81002, "connection is down" }, 
    { 81003, "the rpc has not be initialised" }, 
    { 81004, "argument for message is not a valid type" }, 
    { 81005, "rpc version mismatch" }, 
    { 81006, "packet message out of step" }, 
    { 81007, "not more connections available" }, 
    { 81008, "bad parameter passed to the rpc" }, 
    { 81009, "rpc failed" }, 
    { 81010, "bad number pf arguments for message" }, 
    { 81011, "bad hostname, or host not responding" }, 
    { 81012, "rpc failed to fork service correctly" }, 
    { 81013, "cannot find or open the unirpcserices file" }, 
    { 81014, "unable to find the service in the unirpcservices file" }, 
    { 81015, "connection has timed out" }, 
    { 81016, "connection refused, unirpcd not running" }, 
    { 81017, "Failed to initialise the socket" }, 
    { 81018, "The unirpcd service on the server has been paused" }, 
    { 81019, "Invalid transport type" }, 
    { 81020, "Bad pipe" }, 
    { 81021, "Error writing to pipe" }, 
    { 81022, "Error reading from pipe" }, 
    { 81029, "Bad SSL mode flag" }, 
    { 81030, "SSL mode not supported" }, 
    { 81031, "Bad SSL property list name/password" }, 
    { 81032, "SSL handshake failed" }, 
    { 81033, "Error SSL read/write operation" }
};

