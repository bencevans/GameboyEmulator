#!/bin/bash


# Install build 'stuff'
sudo apt-get install cmake codelite g++ libsdl2-dev
# Install SFML and dependencies
cmake -G "CodeLite - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
