#!/bin/bash

set -e
set -x

if which pyenv > /dev/null; then
    eval "$(pyenv init -)"
fi
pyenv activate conan

mkdir build
cd build && conan install .. --build && cmake .. && make && ctest
#python build.py
