#!/bin/bash
echo "Building for Wii U..."

if [ -z "$DEVKITPRO" ]; then
	echo "DEVKITPRO environment variable is not set."
	exit 1
fi

echo "Compiling shaders..."
mkdir -p Intermediate/wiiu
Tools/CafeGLSL/glslcompiler.elf \
	-vs Backends/WiiUBackend/Shaders/Shader.vert \
	-ps Backends/WiiUBackend/Shaders/Shader.frag \
	-o Intermediate/wiiu/Shader.gsh

if [ $? -ne 0 ]; then
	echo "Shader compilation failed."
	exit 1
fi

make -f Makefile.wiiu "$@"

if [ $? -ne 0 ]; then
	echo "Wii U build failed."
	exit 1
fi

echo "Wii U build successful."
