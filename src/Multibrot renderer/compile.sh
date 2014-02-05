#!/bin/sh

cmake .
if (make) then
    echo "Compilation success."
    exit 0
else
    echo "Compilation failure."
    exit 1
fi
