#!/bin/bash

bash -c "
mkdir -p build \
&& cd build \
&& cmake .. \
&& make -j $(nproc --all)
"
