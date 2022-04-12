# pick-universe

pick-universe is a nodejs module to interface with UniVerse and UniData databases. This is done through bindings to the functions in the C intercall library provided by Rocket.

## Install

```
> npm install pick-universe
```

This addon requires libffi to be installed. On Ubuntu this will be libffi-dev, on Redhat this will be libffi-devel.

## Examples

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

The output is a string directly from the database. You can convert the string to an array or use the pick-mv library to convert the string.

https://www.npmjs.com/package/pick-mv

This library also has functions to help use multivalue arrays in a way much closer to BASIC than traditional javascript arrays.

Select an entire file and print the ids:

```
const mv = require("pick-mv");
const Universe = require("pick-universe");
const uv = new Universe("localhost", "username", "password", "/path/to/account");

uv.StartSession();

const INV = uv.Open("INVENTORY-FILE");
uv.Select(INV);

while (true) {
    let id = uv.ReadNext();
    if (id === null) break;
    let record = mv.MVMatrix(uv.Read(id, INV));
    console.log(id, record[[1]]);
}

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

## Changelog

0.2.1 - Return errors for subroutines.

0.2.0 - Record IDs get converted to ISO8859-1 just like records. Error codes are accompanied by their messages from the intercall header file.

## Hacking Instructions

To hack on this addon, you can run the following commands:

```
> git clone git@github.com:Krowemoh/node-pick-universe.git
> cd node-pick-universe
> npm install
> node-gyp configure build
```
