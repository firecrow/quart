#!/bin/bash

if [ "$1" = test ]; then
    clang-9 -ggdb -o test test.c -DTEST=1
else
    clang-9 -ggdb -o run quart.c
fi
