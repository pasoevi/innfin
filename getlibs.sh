#!/bin/bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=linux;;
    Darwin*)    machine=mac;;
    *)          machine="UNKNOWN:${unameOut}"
esac

curl -LO https://github.com/pasoev/innfin/releases/download/v0.0.4/libs_${machine}.tar.gz
tar xf libs_${machine}.tar.gz
ln -s lib32 lib
