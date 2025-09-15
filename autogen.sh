#!/bin/sh

# This script regenerates the autotools build system

echo "Running autoreconf..."
autoreconf --force --install --verbose

echo
echo "Autotools setup complete. You can now run:"
echo "./configure"
echo "make"
echo "make install (as root if needed)"