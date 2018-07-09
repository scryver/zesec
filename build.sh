#!/bin/bash

set -e

curDir="$(pwd)"
codeDir="$curDir/src"
codeTestDir="$curDir/test"
buildDir="$curDir/gebouw"
buildTestDir="$buildDir/tests"

flags="-O0 -g -ggdb -Wall -Werror -pedantic"
exceptions="-Wno-unused-function"

mkdir -p "$buildTestDir"

pushd "$buildTestDir" > /dev/null

clang $flags $exceptions "$codeTestDir/test_tokenizer.c" -o test_tokenizer
clang $flags $exceptions "$codeTestDir/test_parser.c" -o test_parser
clang $flags $exceptions "$codeTestDir/test_interpreter.c" -o test_interpreter

./test_tokenizer || echo "Token Failed"
./test_parser || echo "Parser Failed"
./test_interpreter || echo "Interpreter Failed"

popd > /dev/null

pushd "$buildDir" > /dev/null

clang $flags $exceptions "$codeDir/main_tokenizer.c" -o tokenizer
clang $flags $exceptions "$codeDir/main_parser.c" -o parser
clang $flags $exceptions "$codeDir/main_interpreter.c" -o interpreter

popd > /dev/null

bash cleaner.sh
