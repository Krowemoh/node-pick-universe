# node-pick-universe

This is an extremely immature package and really more of a learning excersise than anything else. The goal was to be able to call universe subroutines from node and this does work. There are probably a variety of issues that I still need to take a look at but I posted this here so people can get an idea of how to marry node and universe together.

## Build Instructions

You'll most likely need to build this package from source. 


### Requirements
- a compiler that can compile C++17. (Which on rhel 7 was a bit of a pain to set up.)
- node-gyp 
- libffi headers (libffi-devel on rhel or libffi-dev on ubuntu)
- libuvic and intcall.h to use intercall (I've included these in the repo)

There are two libraries that are needed, libuvic and libffi. libuvic is provided by Rocket and I've added that to the lib folder in this repo.

Once you have these things, you can then run the following command to build the addon.

```
❯ node-gyp configure build
```

Good luck!

## Installation Instructions

Once the addon is built, you can copy the build/Release/universe-native.node file to wherever you want and simply require it in a javascript file to begin calling universe subroutines.

You can test this library by using the index.js provided.

One the BASIC side, you have a program called TEST.NIV which contains:
```
SUBROUTINE TEST.NIV(ARG1, ARG2, ARG3, ARG4, ARG5)
ARG1 = "ZERO" : @SVM : "ATTR1"
ARG2 = "ONE"
ARG3 = "TWO"
ARG4 = "THREE"
ARG5 = "FOUR"
```

## API

This is straightforward translation of how the intercall library works.

### StartSession

This function will start a universe session, if it can't, it will throw an error. The session id can be used in SetSession to change which session is currently active.

```
const session_id = universe.StartSession();
```

### EndSession

This will close the current session and release any locks that are active.

```
universe.EndSession();
```

### Open

This will return a file handler if the file exists, otherwise it will throw an error.

```
const INVENTORY_FILE = universe.Open("INVENTORY-FILE");
```

### Close

This function will close and release any locks associated with the file handler.

```
universe.Close(INVENTORY_FILE);
```

### Read

Read in an entire record by giving an ID and a file handler.

```
const x = universe.Read("ITEM.ID", INVENTORY_FILE);
```
