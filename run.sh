#!/bin/bash

set -e

gcc *.c -lGL -lGLEW -lglfw -lm -o opengl
./opengl
