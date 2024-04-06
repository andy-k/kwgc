# KWGC

## About

KWGC is an implementation of the Kurnia Word Graph Compiler.

It is based on the original Rust implementation in Wolges, translated when KWG
had only one layout, which is now called Legacy.

That Rust version now offers other layouts, and the Legacy layout is no longer
preferred.

Only the Legacy layout is currently implemented in KWGC.

Please use the Rust implementation. This implementation in C is slower, even
for the Legacy layout.

## License

Copyright (C) 2020-2024 Andy Kurnia.\
Released under the MIT license.

Bugs included.

## Related Projects

- https://github.com/andy-k/wolges

## Usage

`make`, then `./kwgc` to get usage instructions.
