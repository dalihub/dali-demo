#!/bin/bash

INSTALL_PATH=$DESKTOP_PREFIX/share/emscripten/

echo "Installing Emscripten examples to: $INSTALL_PATH"

mkdir -p $INSTALL_PATH
cp -v *.{js,html} $INSTALL_PATH/

