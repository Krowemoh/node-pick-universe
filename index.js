require("dotenv").config();

const Universe = require('./build/Release/universe-native');
var universe = new Universe.Universe(process.env.U_HOST, process.env.U_USERNAME, process.env.U_PASSWORD, process.env.U_ACCOUNT);

var arg1 = "0";
var arg2 = "1";
var arg3 = "2";
var arg4 = "3";
var arg5 = "4";

var subroutine = "TEST.NIV"

var result = universe.CallSubroutine(subroutine, arg1, arg2, arg3, arg4, arg5);

console.log("Result: " , result);
