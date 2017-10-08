#!/usr/bin/bash

cd build/
cp Release/*.exe .
cd ../release
cp -r ../build ./innfin_0.0.1
tar czf innfin_0.0.1.tar.gz innfin_0.0.1
# zip czf innfin_0.0.1.tar.gz innfin_0.0.1


