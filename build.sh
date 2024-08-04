#!/bin/bash

premake5 gmake2

if [ $? -ne 0 ]; then
    echo "premake5 gmake2 failed"
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

cp -r Editor/Assets out/bin/Debug-linux-x86_64/Editor

if [ $? -ne 0 ]; then
    echo "Failed to copy Assets directory to out/bin/Debug-linux-x86_64/Editor"
    exit 1
fi
