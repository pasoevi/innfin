#!/bin/bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=linux;;
    Darwin*)    machine=mac;;
    MSYS_NT*)    machine=win64;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

echo "https://github.com/pasoevi/innfin/releases/download/v0.0.5/libs_${machine}.tar.gz"

curl -LO https://github.com/pasoevi/innfin/releases/download/v0.0.5/libs_${machine}.tar.gz
tar xf libs_${machine}.tar.gz

if [[ ! -d lib ]]
then
    ln -s lib32 lib
fi
