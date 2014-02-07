#!/bin/sh

if (g++ -O3 --std=c++11 main.cpp -o converter) then
    echo "Compilation success."
    exit 0
else
    echo "Compilation failure."
    exit 1
fi
