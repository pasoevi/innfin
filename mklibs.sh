#!/bin/bash



curl -LO https://github.com/pasoev/innfin/releases/download/v0.0.4/libs_linux.tar.gz
tar xf libs_linux.tar.gz
ln -s lib32 lib
