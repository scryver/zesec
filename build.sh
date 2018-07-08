#!/bin/bash

curDir="$(pwd)"
codeDir="$curDir/src"
buildDir="$curDir/gebouw"

flags="-O0 -g -ggdb -Wall -Werror -pedantic"

mkdir -p "$buildDir"

pushd "$buildDir" > /dev/null

clang $flags "$codeDir/tokenizer.c" -o tokenizer

popd > /dev/null

bash cleaner.sh
