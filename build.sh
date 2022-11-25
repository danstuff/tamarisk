#!/bin/bash

printf "\nBuilding allium...\n\n"
cd build
cmake ..
cmake --build .
