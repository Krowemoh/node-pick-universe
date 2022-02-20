# pick-universe

pick-universe is a nodejs module to interface with UniVerse and UniData databases. This is done through bindings to the majority of the functions in the C intercall library provided by Rocket.

## Install

```
> npm install pick-universe
```

## Build Instructions

If you have issues with the installation, you can build this project from source. To build from source you'll need a compiler that can compile C++17, I used gcc 9.3. You will also need to install node-gyp.

This addon uses 2 statically linked libraries, libuvic.a and libffi. If you have issues with libffi, you can use your system libffi instead of the statically linked version.

Update bindings.gyp with the following.

```
    "libraries": [ "../libs/libuvic.a", "-lffi" ],
```

To build the addon, run the following commands.

```
> git clone git@github.com:Krowemoh/node-pick-universe.git
> cd node-pick-universe
> npm install
> node-gyp configure build
```

Once the addon is built, you can copy the build/Release/universe-native.node and index.js file to where you need it.
