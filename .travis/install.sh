#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Darwin' ]]; then
    brew update || brew update
    brew outdated pyenv || brew upgrade pyenv
    brew install pyenv-virtualenv
    brew install cmake || true
else
    curl -L https://raw.githubusercontent.com/yyuu/pyenv-installer/master/bin/pyenv-installer | bash
fi

if which pyenv > /dev/null; then
    eval "$(pyenv init -)"
fi

pyenv install 2.7.10
pyenv virtualenv 2.7.10 conan
pyenv rehash
pyenv activate conan

pip install conan --upgrade
pip install conan_package_tools

conan user
