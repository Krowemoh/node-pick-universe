const universe = require('./build/Release/universe-native');

class Universe {
    constructor(host, username, password, account) {
        this._uv = new universe.Universe(host, username, password, account);
    }

    CallSubroutine(...args) {
        return this._uv.CallSubroutine.apply(args);
    }

    ClearData() {
        return this._uv.ClearData();
    }

    ClearFile(file) {
        return this._uv.ClearFile(file);
    }

    ClearSelect(listNumber = 0) {
        return this._uv.ClearSelect(listNumber);
    }

    Close(file) {
        return this._uv.Close(file);
    }

    ContinueExecution(reply) {
        return this._uv.ContinueExecution(reply);
    }

    Data(inputString) {
        return this._uv.Date(inputString);
    }

    Date() {
        return this._uv.Date();
    }

    Delete(record, file) {
        return this._uv.Delete(record, file);
    }

    DeleteField(string, field = 0, value = 0, subvalue = 0) {
        return this._uv.DeleteField(string, field, value, subvalue);
    }

    EndAllSessions() {
        return this._uv.EndAllSessions();
    }

    EndSession() {
        return this._uv.EndSession();
    }

    Execute(command) {
        return this._uv.Execute(command);
    }

    Extract(record, field = 0, value = 0, subvalue = 0) {
        return this._uv.Extract(record, field, value, subvalue);
    }

    FileInfo(file, key = 0) {
        return this._uv.FileInfo(file);
    }

    FileLock(file) {
        return this._uv.FileLock();
    }

    FileUnlock(file) {
        return this._uv.FileUnlock();
    }

    Format(record, format) {
        return this._uv.Format(record, format);
    }

    FormList(string, listNumber = 0) {
        return this._uv.FormList(string, listNumber);
    }

    GetList(name, listNumber = 0) {
        return this._uv.GetList(name, listNumber);
    }

    GetLocale(key = 0) {
        return this._uv.GetLocale(key);
    }

    GetValue(key = 0) {
        return this._uv.GetValue(key);
    }

    ICONV(string, conv) {
        return this._uv.ICONV(record, conv);
    }

    Indices(file, index) {
        return this._uv.Indices(file, index);
    }

    Insert(record, string, field = 0, value = 0, subvalue = 0) {
        return this._uv.Insert(record, string, field, value, subvalue);
    }

    IsAlpha(string) {
        return this._uv.IsAlpha(string);
    }

    Locate(needle, haystack, order, start = 1, field = 0, value = 0) {
        return this._uv.Locate(needle, haystack, order, start, field, value);
    }

    Lock(lock) {
        return this._uv.Lock(lock);
    }

    Lower(string) {
        return this._uv.Lower(string);
    }

    OCONV(string, conv) {
        return this._uv.OCONV(string, conv);
    }

    Open(file) {
        return this._uv.Open(file);
    }

    Raise(string) {
        return this._uv.Raise(string);
    }

    Read(id, file) {
        return this._uv.Read(id, file);
    }

    ReadList(listNumber = 0) {
        return this._uv.ReadList(listNumber);
    }

    ReadNext(listNumber = 0) {
        return this._uv.ReadNext(listNumber);
    }

    ReadValue(id, field, file) {
        return this._uv.ReadValue(id, field, file);
    }

    RecordLock(id, file) {
        return this._uv.RecordLock(id, file);
    }

    RecordLocked(id, file) {
        return this._uv.RecordLocked(id, file);
    }

    Release(id, file) {
        return this._uv.Release(id, file);
    }

    Remove(record, delimiter) {
        return this._uv.Remove(record, delimiter);
    }

    Replace(record, replace, field = 0, value = 0, subvalue = 0) {
        return this._uv.Replace(record, replace, field, value, subvalue);
    }

    RunIType(record, filename, itype) {
        return this._uv.RunIType(record, filename, itype);
    }

    Select(file, listNumber = 0) {
        return this._uv.Select(file, listNumber);
    }

    SelectIndex(index, value, file) {
        return this._uv.SelectIndex(index, value, file);
    }

    SessionInfo(key) {
        return this._uv.SessionInfo(key);
    }

    SetSession(session) {
        return this._uv.SetSession(session);
    }

    SetTimeout(timeout) {
        return this._uv.SetTimeout(timeout);
    }

    SetValue(key = 0, value) {
        return this._uv.SetValue(key, value);
    }

    StartSession() {
        return this._uv.StartSession();
    }

    Time() {
        return this._uv.Time();
    }

    TimeDate() {
        return this._uv.TimeDate();
    }

    Trans(record, filename, field = -1) {
        return this._uv.Trans(record, filename, field);
    }

    Unlock(lock) {
        return this._uv.Unlock(lock);
    }

    Write(record, id, file) {
        return this._uv.Write(record, id, file);
    }

    WriteValue(string, id, field, file) {
        return this._uv.WriteValue(string, id, field, file);
    }
}

module.exports = Universe; 
