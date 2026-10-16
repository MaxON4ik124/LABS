# Vendored zlib

- Version: 1.3.2
- Source: https://zlib.net/fossils/zlib-1.3.2.tar.gz
- Archive SHA-256: bb329a0a2cd0274d05519d61c667c062e06990d72e125ee2dfa8de64f0119d16
- Downloaded: 2026-10-13
- License: see `zlib/LICENSE`.

The upstream sources are in `zlib/`. `comp.bat` builds the static library
with the existing w64devkit compiler into `build/zlib-mingw/libzs.a`.
Generated headers and build artifacts stay in `build/`.
No downloads or global dependency installation are needed on subsequent builds.
