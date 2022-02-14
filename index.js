require("dotenv").config();

const delimiterCodes = [254, 253, 252, 251, 250];
const delimiters = delimiterCodes.map(d => String.fromCharCode(d));

function ArrayToMV(arr, depth = 0) {
    if (!Array.isArray(arr) || depth > 5) return arr;
    let delim = delimiters[depth];
    return arr.map(a => ArrayToMV(a, depth+1)).join(delim);
}

function MVToArray(string, depth = 0) {
    if (depth >= 5) return string;
    let delim = delimiters[depth];

    if (delimiters.some(d => string.indexOf(d) !== -1)) {
        let arr = string.split(delim).map(a => MVToArray(a, depth+1));
        return arr;

    } else {
        if (depth === 0) {
            if (string === "") {
                return [];
            }
            return [string];
        } else {
            return MVToArray(string, depth+1)
        }
    }
}

const Universe = require('./build/Release/universe-native');
var universe = new Universe.Universe(process.env.U_HOST, process.env.U_USERNAME, process.env.U_PASSWORD, process.env.U_ACCOUNT);

universe.StartSession();

var arg1 = ["0", "1"];
var arg2 = "1";
var arg3 = "2";
var arg4 = "3";
var arg5 = "4";

var subroutine = "TEST.NIV"

var result = universe.CallSubroutine(subroutine, ArrayToMV(arg1), arg2, arg3, arg4, arg5);

universe.EndSession();

console.log(result[0]);

result = result.map(r => MVToArray(r));

console.log(JSON.stringify(result[0]));
console.log(ArrayToMV(result[0]));

