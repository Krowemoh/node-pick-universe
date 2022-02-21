# pick-universe

pick-universe is a nodejs module to interface with UniVerse and UniData databases. This is done through bindings to the functions in the C intercall library provided by Rocket.

## Install

```
> npm install pick-universe
```

This addon requires libffi to be installed. On Ubuntu this will be libffi-dev, on Redhat this will be libffi-devel.

## Quick Example

```
const Universe = require("pick-universe");
const uv = new Universe("localhost", "username", "password", "/path/to/account");

uv.StartSession();

// Read in a record
const VOC = uv.Open("VOC");
const time = uv.Read("TIME", VOC);
console.log(time);

// Execute a command
const [result, output] = uv.Execute("CT MD TIME");
console.log(output);

uv.EndAllSessions();
```

## Documentation

Documentation is available [online](https://nivethan.dev/documentation/pick-universe).

You can generate documentation by running the generate-docs command.

```
> npm install -g jsdocs
> npm install --save-dev minami
> jsdoc index.js README.md -t node_modules/minami/
```

This will create the documentation in a folder called out.

## Hacking Instructions

To hack on this addon, you can run the following commands:

```
> git clone git@github.com:Krowemoh/node-pick-universe.git
> cd node-pick-universe
> npm install
> node-gyp configure build
```
