#!/bin/bash
set -ex

mkdir -p build
cd build
CXX=g++-5 CC=gcc-5 cmake ..
make
./spring
