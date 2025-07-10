#!/bin/bash

bindgen cpp-src/headers_wrapper.h -o core.rs -- -x c++
sed -i '1s/^/#![allow(warnings)]\n/' src/core.rs
