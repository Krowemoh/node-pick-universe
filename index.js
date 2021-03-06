const _Universe = require("./build/Release/universe-native");

/** Class for all the universe functions */
class Universe {
    /**
     * Create a Universe object.
     *
     * @param {string} hostname - Hostname or ip address for the universe server. 
     * @param {string} username - Username to use to log in.
     * @param {string} password - Password to use to log in.
     * @param {string} account - Account to log in to.
     */
    constructor(host, username, password, account) {
        this._uv = new _Universe.Universe(host, username, password, account);
    }

    /**
     * Call a cataloged subroutine.
     *
     * @param {string} subroutine - The name of the subroutine.
     * @param {...args} args - The arguments to be passed to the subroutine.
     * @return {array} The args are returned with their changes.
     * 
     */
    CallSubroutine(subroutine, ...args) {
        args.unshift(subroutine);
        return this._uv.CallSubroutine.apply(this._uv, args);
    }

    /**
     * Flushes data loaded by the Data function..
     *
     */
    ClearData() {
        return this._uv.ClearData();
    }

    /**
     * Clears a file of all its records.
     *
     * @param {number} file - The file id to be cleared. 
     */
    ClearFile(file) {
        return this._uv.ClearFile(file);
    }

    /**
     * Clears an active list.
     *
     * @param {number} [listNumber=0] - The list to be cleared.
     */
    ClearSelect(listNumber = 0) {
        return this._uv.ClearSelect(listNumber);
    }

    /**
     * Closes an open file. This will also release all locks for the file that this user has.
     *
     * @param {number} file - The file id to be closed. 
     */
    Close(file) {
        return this._uv.Close(file);
    }

    /**
     * Continue an execute that requested input.
     *
     * @param {string} input - The input to be used to continue executing a command.
     */
    ContinueExecution(input) {
        return this._uv.ContinueExecution(input);
    }

    /**
     * Load a string into the input stack. This will be used by input statements in a BASIC program.
     *
     * @param {number} str - The string to be added to the queue. 
     */
    Data(str) {
        return this._uv.Data(str);
    }

    /**
     * Get the server system date in internal format.
     *
     * @return {number} Days since Dec 31 1967. 
     */
    Date() {
        return this._uv.Date();
    }

    /**
     * Delete a record from an open file.
     *
     * @param {string} record_id - The id of the record you want to delete.
     * @param {string} file - The file you want to delete from.
     */
    Delete(record_id, file) {
        return this._uv.Delete(record_id, file);
    }

    /**
     * Delete a value based on field, value and/or subvalue.
     *
     * @param {string} str - The string you want to delete from.
     * @param {number} [field=0] - The field position.
     * @param {number} [value=0] - The value position.
     * @param {number} [subvalue=0] - The subvalue position.
     * @return {string} The new string after removal.
     */
    DeleteField(str, field = 0, value = 0, subvalue = 0) {
        return this._uv.DeleteField(str, field, value, subvalue);
    }

    /**
     * End all sessions.
     *
     */
    EndAllSessions() {
        return this._uv.EndAllSessions();
    }

    /**
     * End the current session.
     *
     */
    EndSession() {
        return this._uv.EndSession();
    }

    /**
     * Execute a database command.
     *
     * @param {string} command - The command to execute on the database.
     * @return {string} The output of the command is returned.
     */
    Execute(command) {
        return this._uv.Execute(command);
    }

    /**
     * Extract a field, value or subvalue from a string.
     *
     * @param {string} str - The string that you want to extract from.
     * @param {number} [field=0] - The field to extract from.
     * @param {number} [value=0] - The value to extract from.
     * @param {number} [subvalue=0] - The subvalue to extract from.
     * @return {string} The substring that was extracted.
     */
    Extract(str, field = 0, value = 0, subvalue = 0) {
        return this._uv.Extract(str, field, value, subvalue);
    }

    /**
     * Get information about an open file.
     *
     * @param {number} file - The file you want to get information on.
     * @param {number} [key=0] - The information you wish to get.
     * @return {string} The information requested.
     */
    FileInfo(file, key = 0) {
        return this._uv.FileInfo(file);
    }

    /**
     * Lock an open file.
     *
     * @param {number} file - The file you want to lock.
     */
    FileLock(file) {
        return this._uv.FileLock();
    }

    /**
     * Unlock a locked file.
     *
     * @param {number} file - The file you want to unlock.
     */
    FileUnlock(file) {
        return this._uv.FileUnlock();
    }

    /**
     * Format a string.
     *
     * @param {string} str - The string to be formatted.
     * @param {string} format - The format to be used.
     */
    Format(str, format) {
        return this._uv.Format(str, format);
    }

    /**
     * Create a select list from a list of ids.
     *
     * @param {string} list - List of record ids.
     * @param {number} [slot=0] - The slot to load the select into.
     */
    FormList(list, slot = 0) {
        return this._uv.FormList(list, slot);
    }

    /**
     * Get a list and load it to an active list slot.
     *
     * @param {string} name - The name of the list to restore.
     * @param {number} [slot=0] - The slot to load the restored list into.
     */
    GetList(name, slot = 0) {
        return this._uv.GetList(name, slot);
    }

    /**
     * Get the value of a system variable.
     *
     * @param {number} [key=0] - The system variable to read.
     */
    GetValue(key = 0) {
        return this._uv.GetValue(key);
    }

    /**
     * Convert a string to an internal format using the conversion.
     *
     * @param {string} str - The string to convert.
     * @param {string} conv - The conversion code.
     * @return {string} The converted string.
     */
    ICONV(str, conv) {
        return this._uv.ICONV(str, conv);
    }

    /**
     * Get information about an index on an open file.
     *
     * @param {number} file - The file to check.
     * @param {string} [index=""] - The index to get information about, if this is blank, all indices will be returned.
     */
    Indices(file, index="") {
        return this._uv.Indices(file, index);
    }

    /**
     * Insert a substring into a string specified by the field, value and/or subvalue.
     *
     * @param {string} str - The string to insert into.
     * @param {string} substring - The substring to be inserted.
     * @param {number} [field=0] - The field position.
     * @param {number} [value=0] - The value position.
     * @param {number} [subvalue=0] - The subvalue position.
     * @return {string} The new string after insertion.
     */
    Insert(str, substring, field = 0, value = 0, subvalue = 0) {
        return this._uv.Insert(str, substring, field, value, subvalue);
    }

    /**
     * Determines if a string is alphabetic.
     *
     * @param {string} str - String to be tested.
     * @return {boolean} True or false if the string is alphabetic. 
     */
    IsAlpha(str) {
        return this._uv.IsAlpha(str) === 1 ? true : false;
    }

    /**
     * Search a dynamic array for a string.
     *
     * @param {string} needle - The search string.
     * @param {string} haystack - The array to search in.
     * @param {string} order - The ordering of the array.
     * @param {number} [start=1] - The position to start searching from.
     * @param {number} [field=0] - The field position.
     * @param {number} [value=0] - The value position.
     * @param {number} [subvalue=0] - The subvalue position.
     */
    Locate(needle, haystack, order, start = 1, field = 0, value = 0) {
        return this._uv.Locate(needle, haystack, order, start, field, value);
    }

    /**
     * Set a semaphore or public process lock. There are 0 - 63 locks available.
     *
     * @param {number} lock - The lock number.
     */
    Lock(lock) {
        return this._uv.Lock(lock);
    }

    /**
     * Convert delimiters to the next lower delimiter.
     *
     * @param {string} str - The string to convert.
     * @return {string} The converted string. 
     */
    Lower(str) {
        return this._uv.Lower(str);
    }

    /**
     * Convert a string to an external format using the conversion.
     *
     * @param {string} str - The string to convert.
     * @param {string} conv - The conversion code.
     * @return {string} The converted string.
     */
    OCONV(string, conv) {
        return this._uv.OCONV(string, conv);
    }

    /**
     * Open a file in the database.
     *
     * @param {string} filename - The name of the file to open.
     * @return {number} The file handler.
     */
    Open(filename) {
        return this._uv.Open(filename);
    }

    /**
     * Convert delimiters to the next higher delimiter.
     *
     * @param {string} str - The string to convert.
     * @return {string} The converted string. 
     */
    Raise(string) {
        return this._uv.Raise(string);
    }

    /**
     * Read a record from an open file.
     *
     * @param {string} id - The id of the record.
     * @param {number} file - The file handler returned by a previous open.
     * @return {string} The record that was read in.
     */
    Read(id, file) {
        return this._uv.Read(id, file);
    }

    /**
     * Reads all the records in an active list.
     *
     * @param {number} file - The file handler returned by a previous open.
     * @param {number} list_number - The list to read ids from.
     * @return {Array} An array of records.
     */
    ReadAll(file, list_number = 0) {
        return this._uv.ReadAll(file, list_number);
    }

    /**
     * Convert an active list into a list of ids.
     *
     * @param {number} [slot=0] - The select list slot number to read from.
     * @return {string} A list of ids.
     */
    ReadList(slot = 0) {
        return this._uv.ReadList(slot);
    }

    /**
     * Read the next id from an active list.
     *
     * @param {number} [slot=0] - The select list slot number to read from.
     * @return {string} An id.
     */
    ReadNext(slot = 0) {
        return this._uv.ReadNext(slot);
    }

    /**
     * Read a specific value from a record.
     *
     * @param {string} id - The id of the record.
     * @param {number} field - The field to read from.
     * @param {number} file - The file to read from.
     * @return {string} The value that was read.
     */
    ReadValue(id, field, file) {
        return this._uv.ReadValue(id, field, file);
    }

    /**
     * Lock a record in an open file.
     *
     * @param {string} id - The id of the record to lock.
     * @param {number} file - The file the record is in.
     */
    RecordLock(id, file) {
        return this._uv.RecordLock(id, file);
    }

    /**
     * Check what the lock status is of a record.
     *
     * @param {string} id - The id of the record to check.
     * @param {number} file - The file the record is in.
     * @return {number} The lock status. 
     */
    RecordLocked(id, file) {
        return this._uv.RecordLocked(id, file);
    }

    /**
     * Release any locks on a record.
     *
     * @param {string} id - The id of the record to release.
     * @param {number} file - The file the record is in.
     */
    Release(id, file) {
        return this._uv.Release(id, file);
    }

    /**
     * Replace a value specified by field, value and subvalue with a substring.
     *
     * @param {string} str - The string to do the replacement in.
     * @param {string} substring - The replacement substring.
     * @param {number} [field=0] - The field position.
     * @param {number} [value=0] - The value position.
     * @param {number} [subvalue=0] - The subvalue position.
     */
    Replace(str, substring, field = 0, value = 0, subvalue = 0) {
        return this._uv.Replace(str, substring, field, value, subvalue);
    }

    /**
     * Execute a I type dictionary item on a record. I types allow for subroutines to run on records from a dictionary.
     *
     * @param {string} record_id - The id of the record you want to use.
     * @param {string} filename - The name of the file you want to read the record from.
     * @param {string} itype - The name of the I type record in the dictionary.
     * @return {string} The value generated by the I type.
     */
    RunIType(record, filename, itype) {
        return this._uv.RunIType(record, filename, itype);
    }

    /**
     * Create a list of all record ids from an open file and place it in an list slot.
     *
     * @param {number} file - The file to select.
     * @param {number} [slot=0] - The list slot number to place the ids in.
     */
    Select(file, slot = 0) {
        return this._uv.Select(file, slot);
    }

    /**
     * Create a list using an index.
     *
     * @param {number} index - The index to use.
     * @param {string} value - The value of the index, this is what will be checked when selecting record ids.
     * @param {number} file - The file to select from.
     */
    SelectIndex(index, value, file) {
        return this._uv.SelectIndex(index, value, file);
    }

    /**
     * Get information about the current session.
     *
     * @param {number} key - The information being requested.
     * @return {string}  The information requested.
     */
    SessionInfo(key) {
        return this._uv.SessionInfo(key);
    }

    /**
     * Change the universe session.
     *
     * @param {number} session - The session id to switch to.
     */
    SetSession(session) {
        return this._uv.SetSession(session);
    }

    /**
     * Set the timeout for the RPC port in seconds. The default is 24 hours.
     *
     * @param {number} timeout - The timeout in seconds.
     */
    SetTimeout(timeout) {
        return this._uv.SetTimeout(timeout);
    }

    /**
     * Set the value of a system variable. Currently only 7 is available.
     *
     * @param {number} key - The system variable to set.
     * @param {string} value - The value to set the system variable to.
     */
    SetValue(key = 7, value) {
        return this._uv.SetValue(key, value);
    }

    /**
     * Start a universe session.
     *
     * @return {number} The session id is returned and can be used in changing sessions.
     */
    StartSession() {
        return this._uv.StartSession();
    }

    /**
     * Get the server time in internal format. Internal format is seconds since midnight.
     *
     */
    Time() {
        return this._uv.Time();
    }

    /**
     * Get the time and date as hh:mm:ss dd mmm yyyy.
     * 
     */
    TimeDate() {
        return this._uv.TimeDate();
    }

    /**
     * Read a record in without opening a file.
     * 
     * @param {string} id - The id of the record to be read in.
     * @param {string} filename - The name of the file to be read.
     * @param {number} [field=-1] - The field to be read in, -1 will read in the entire record.
     * @return {string} The value or record being requested.
     */
    Trans(id, filename, field = -1) {
        return this._uv.Trans(id, filename, field);
    }

    /**
     * Unlock a semaphore(pulic process) lock.
     * 
     * @param {number} lock - The lock number to release.
     */
    Unlock(lock) {
        return this._uv.Unlock(lock);
    }

    /**
     * Write a record to a file.
     * 
     * @param {string} record - The record to write.
     * @param {string} id - The id of the record.
     * @param {number} file - The file to write to.
     */
    Write(record, id, file) {
        return this._uv.Write(record, id, file);
    }

    /**
     * Write a value to a file.
     * 
     * @param {string} str - The value to write.
     * @param {string} id - The id of the record.
     * @param {number} field - TThe field to write to.
     * @param {number} file - The file to write to.
     */
    WriteValue(str, id, field, file) {
        return this._uv.WriteValue(str, id, field, file);
    }
}

module.exports = Universe;
