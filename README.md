# node-pick-universe

This is an extremely immature package and really more of a learning excersise than anything else. The goal was to be able to call universe subroutines from node and this does work. There are probably a variety of issues that I still need to take a look at but I posted this here so people can get an idea of how to marry node and universe together.

## Build Instructions

You'll most likely need to build this package from source. 


### Requirements
- a compiler that can compile C++17. (Which on rhel 7 was a bit of a pain to set up.)
- node-gyp 
- libffi headers (libffi-devel on rhel or libffi-dev on ubuntu)

There are two libraries that are needed, libuvic and libffi. libuvic is provided by Rocket and I've added that to the lib folder in this repo. libffi needs to be installed by your package manager and you can find it by doing the following:

```
❯ ldconfig  -p | grep ffi
        libffi.so.6 (libc6,x86-64) => /lib64/libffi.so.6
```

Copy this path and update the binding.gyp file.

```
"libraries": [ "../lib/libuvic.a", "/lib64/libffi.so.6" ],
```

With that you should be able to now run the node-gyp command to build the addon.

```
❯ node-gyp configure build
```

Good luck!

## Installation Instructions

Once the addon is built, you can copy the build/Release/universe-native.node file to wherever you want and simply require it in a javascript file to begin calling universe subroutines.

For example: 
```
require("dotenv").config();

const Universe = require('./universe-native');
var universe = new Universe.Universe(process.env.U_HOST, process.env.U_USERNAME, process.env.U_PASSWORD, process.env.U_ACCOUNT);

var arg1 = "";
var arg2 = "John";
var result = universe.CallSubroutine("NIVT", arg1, arg2, "");

console.log("Arg 1: " + arg1);
console.log("Arg 2: " + arg2);
console.log("Result: " , result);
```
