# Asset Module

This is a tool for embedding compressed files into Lua binary modules.  It's useful for instances in which the license terms for third-party assets require those assets to be distributed in an obfuscated format.

## Usage

`build.sh <module-name> <asset-file-path> <out-dir>`

Example:

`./build.sh sprites ./images/spritesheet.png ./lib`

This would result in a Lua module library `sprites.so` (`sprites.dll` on windows) being built and copied into the `./lib` directory.

This module can then be imported in Lua the usual way:

```lua
-- decompress and load the file into memory
local sprites = require 'sprites'
```

The result of the `require` is a table with the following fields:
* An integer `length` field indicating the size of the uncompressed data
* A field `buffer` that consists of a Lua "lightuserdata" which is a pointer to the decompressed data
