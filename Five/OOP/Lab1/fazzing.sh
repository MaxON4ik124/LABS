#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

command -v afl-clang-fast++ >/dev/null
command -v afl-fuzz >/dev/null

test -f input/seed1.txt

if [ -d output/default ]; then
    echo "output/default уже существует."
    echo "Для нового запуска очистите output или используйте отдельный каталог."
    echo "Продолжение существующего запуска: afl-fuzz -i - -o output -- ./build/fazz_afl"
    exit 1
fi

mkdir -p build

echo "[1/3] Сборка MyString.o с AFL-инструментацией..."
afl-clang-fast++ \
    -std=c++17 -O0 -g -fno-inline \
    -c MyString.cpp \
    -o build/MyString.o

echo "[2/3] Сборка harness..."
afl-clang-fast++ \
    -std=c++17 -O0 -g -fno-inline \
    fazz.cpp build/MyString.o \
    -o build/fazz_afl

echo "[3/3] Запуск AFL++ с seed input/seed1.txt..."
export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1

exec afl-fuzz \
    -i input \
    -o output \
    -m none \
    -- ./build/fazz_afl
