require("dotenv").config();

const Universe = require('./build/Release/universe-native');
var universe = new Universe.Universe(process.env.U_HOST, process.env.U_USERNAME, process.env.U_PASSWORD, process.env.U_ACCOUNT);

let status_matrix = '';
let language_flag = 'EN';
let product = 'BB-AX-01';
let inventory_matrix = '';
let product_type_matrix = '';
let colour_matrix = '';
let sku_list = '';

var result = universe.CallSubroutine("WEB.GET.INVENTORY.MATRIX", status_matrix, language_flag, product, inventory_matrix, product_type_matrix, colour_matrix, sku_list);

console.log(result);
