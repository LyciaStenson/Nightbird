#!/bin/bash

CONFIG=${1:-editordebug}
PLATFORM=${2:-desktop}

echo "Building Linux project files."
premake5 gmake
if [ $? -ne 0 ]; then
	echo "Failed to build project files."
	exit 1
fi

echo "Building ${CONFIG}_${PLATFORM}..."
make -j$(nproc) config=${CONFIG}_${PLATFORM}
if [ $? -ne 0 ]; then
	echo "Build failed."
	exit 1
fi

echo "Built successfully."
