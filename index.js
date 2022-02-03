require("dotenv").config();

const Universe = require('./build/Release/universe-native');
var universe = new Universe.Universe(process.env.U_HOST, process.env.U_USERNAME, process.env.U_PASSWORD, process.env.U_ACCOUNT);

var arg1 = "";
var arg2 = "Nivethan";
var result = universe.CallSubroutine("NIVT", arg1, arg2);

console.log("Arg 1: " + arg1);
console.log("Arg 2: " + arg2);
console.log("Result: " , result);
