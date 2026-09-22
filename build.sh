#!/bin/bash

set -xe

gcc -Wall -Wextra -O3 -march=native -o bin/build main.c src/VeC.c
