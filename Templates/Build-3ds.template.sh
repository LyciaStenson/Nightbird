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
mkdir -p Intermediate/3ds

make -f Makefile.3ds "$@"

if [ $? -ne 0 ]; then
	echo "3DS build failed."
	exit 1
fi

echo "3DS build successful."
