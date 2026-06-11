#!/bin/bash
echo "Building for 3DS..."

if [ -z "$DEVKITPRO" ]; then
	echo "DEVKITPRO environment variable is not set."
	exit 1
fi

if [ -z "$DEVKITARM" ]; then
	echo "DEVKITARM environment variable is not set."
	exit 1
fi

mkdir -p Binaries/3ds
mkdir -p Intermediate/3ds/data
echo "Compiling shaders..."
picasso -o Intermediate/3ds/data/Shader.v.pica.shbin \
	Backends/3DSBackend/Shaders/Shader.v.pica

if [ $? -ne 0 ]; then
    echo "Shader compilation failed."
    exit 1
fi

make -f Makefile.3ds "$@"

if [ $? -ne 0 ]; then
	echo "3DS build failed."
	exit 1
fi

echo "3DS build successful."
