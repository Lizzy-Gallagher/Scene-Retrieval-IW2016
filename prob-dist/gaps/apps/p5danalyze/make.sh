#!/bin/bash
# Script to make p5danalyze and move it to a testing area

make clean; make
cp ../../bin/x86_64/p5danalyze ../../../bin
