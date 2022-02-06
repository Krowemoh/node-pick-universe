require("dotenv").config();

const Universe = require('./build/Release/universe-native');
var universe = new Universe.Universe(process.env.U_HOST, process.env.U_USERNAME, process.env.U_PASSWORD, process.env.U_ACCOUNT);

var arg1 = "";
var arg2 = "";
var arg3 = "";

var subroutine = "WEB.GET.INVENTORY.MATRIX"
var subroutine = "TEST.NIV"

var result = universe.CallSubroutine(subroutine, arg1, arg2, arg3);

console.log("Result: " , result);
