#!/bin/sh
rm -rf CMakeFiles/
rm -f CMakeCache.txt cmake_install.cmake Makefile install_manifest.txt Multibrot
echo "Successfully cleaned the build directory."

export CXX=/usr/bin/clang++
export CC=/usr/bin/clang

cmake .
cpus=$(grep -c ^processor /proc/cpuinfo)
if (make -j $cpus) then
    ./multibrot
fi
