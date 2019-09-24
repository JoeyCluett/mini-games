#!/bin/bash

g++ -o main main.cpp -std=c++11 -march=native -O3 -ffast-math -I../libs/ -lSDL -lSDL_gfx
