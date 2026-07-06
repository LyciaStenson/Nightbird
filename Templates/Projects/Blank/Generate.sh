#!/bin/bash

CONFIG=${1:-editorrelease}
PLATFORM=${2:-desktop}

case "$CONFIG" in
	editordebug)    CONFIG_DIR="EditorDebug"    ;;
	editorrelease)  CONFIG_DIR="EditorRelease"  ;;
	appdebug)       CONFIG_DIR="AppDebug"       ;;
	apprelease)     CONFIG_DIR="AppRelease"     ;;
	*) echo "Unknown config: $CONFIG"; exit 1   ;;
esac

if [[ -z "$NIGHTBIRD_PATH" ]]; then
	echo "Please set NIGHTBIRD_PATH in your environment."
	exit 1
fi

ENGINE_BINARIES="$NIGHTBIRD_PATH/Binaries/linux-x86_64/$CONFIG_DIR"

NPROJECT_FILE=$(find "$(pwd)" -maxdepth 1 -name "*.nproject" -print -quit)

if [[ -z "$NPROJECT_FILE" ]]; then
	echo "No .nproject found in $(pwd)"
	exit 1
fi

echo "Launching editor with project: $NPROJECT_FILE"

(
	cd "$ENGINE_BINARIES" || exit 1
	./Editor "$NPROJECT_FILE" --generate
)
