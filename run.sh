#!/bin/bash

set -e

gcc opengl.c -lGL -lGLEW -lglfw -lm -o opengl
./opengl
