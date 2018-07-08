#!/bin/bash

curDir="$(pwd)"
codeDir="$curDir/src"
buildDir="$curDir/gebouw"

flags="-O0 -g -ggdb -Wall -Werror -pedantic"
exceptions="-Wno-unused-function"

mkdir -p "$buildDir"

pushd "$buildDir" > /dev/null

clang $flags $exceptions "$codeDir/main_tokenizer.c" -o tokenizer
clang $flags $exceptions "$codeDir/main_parser.c" -o parser

popd > /dev/null

bash cleaner.sh
