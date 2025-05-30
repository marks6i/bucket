#!/bin/bash

# Exit on error
set -e

# Run clang-tidy on all header files except buckets_supp.h
find include/bucket -name "*.h" ! -name "buckets_supp.h" -print0 | while IFS= read -r -d '' file; do
    echo "Checking $file..."
    clang-tidy \
        -p=compile_commands.json \
        --extra-arg=-std=c++20 \
        --extra-arg=-I/usr/include/c++/13 \
        --extra-arg=-I/usr/include/x86_64-linux-gnu/c++/13 \
        --warnings-as-errors=* \
        "$file"
done 