#!/bin/bash
scons -Q
(cd build/ && exec ./innfin.exe)
