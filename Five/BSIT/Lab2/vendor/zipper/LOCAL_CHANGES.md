# Local Windows build

These sources were copied from the existing local Zipper 1.0.4 source tree.
The project uses its own headers and a MinGW-built archive; the globally
installed `libZipper-static.a` contains incompatible Linux ELF objects.

Minizip compatibility fixes present in this copy:

- `ioapi_buf.h` and `ioapi_buf.c`: `fopen64_buf_func` takes `const void*`,
  matching the `open64_file_func` callback.
- `unzip.c`: AES byte fields are read into an `int`, matching
  `unz64local_getByte`, instead of passing a `uLong*`.
- `unzip.c`: the AES strength byte is stored in `aes_encryption_mode`.

The first two fixes were already present in the user's source tree. The
AES strength assignment was restored in this local copy.

`comp.bat` builds this source tree into `build/zipper-mingw`, using the
existing Windows zlib at `C:/MinGW/w64devkit/lib/libz.a`.
