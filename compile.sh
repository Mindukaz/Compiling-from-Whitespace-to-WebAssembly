#!/bin/bash

STR=$1
THING=${STR##*/}
NAME=${THING%.*}

./wswac $1
if [ $? -eq 0 ]; then
    echo Compiled wat
    cd wasm/
    wat2wasm $NAME.wat
    if [ $? -eq 0 ]; then
        echo Compiled wasm
    fi
fi
