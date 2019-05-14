#!/bin/bash


# Install build 'stuff'
sudo apt-get install cmake codelite g++
# Install SFML and dependencies
sudo apt-get install libsfml-dev libfreetype6-dev libvorbis-ocaml-dev libopenal-dev libflac-dev
cmake -G "CodeLite - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
