#!/bin/sh
set -u
ulimit -c 0
./build-math/math_linux_debug assert-zero
result=$?
if [ "$result" -ne 134 ]; then
    echo "Unexpected exit status: $result"
    exit 1
fi
echo "Debug zero-vector assertion confirmed"
for header in utils vec mat geometry; do
    g++ -std=c++20 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -IGame_/src -fsyntax-only "build-math/header_${header}.cpp" || exit 1
done
echo "Standalone headers compile cleanly"
