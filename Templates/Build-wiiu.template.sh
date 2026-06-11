#!/bin/bash
echo "Building for Wii U..."

if [ -z "$DEVKITPRO" ]; then
	echo "DEVKITPRO environment variable is not set."
	exit 1
fi

make -f Makefile.wiiu "$@"

if [ $? -ne 0 ]; then
	echo "Wii U build failed."
	exit 1
fi

echo "Wii U build successful."
