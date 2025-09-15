#!/bin/bash

# This script helps with building setproxy using CMake

# Set default build directory
BUILD_DIR="build"

# Process command line options
while getopts ":hcid:" opt; do
  case ${opt} in
    h)
      echo "Usage: $0 [-c] [-i] [-d BUILD_DIR]"
      echo "  -c: Clean build (removes build directory before building)"
      echo "  -i: Install after building (requires sudo privileges)"
      echo "  -d BUILD_DIR: Specify build directory (default: 'build')"
      echo "  -h: Show this help message"
      exit 0
      ;;
    c)
      CLEAN_BUILD=1
      ;;
    i)
      INSTALL=1
      ;;
    d)
      BUILD_DIR=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

# Clean build if requested
if [ -n "$CLEAN_BUILD" ] && [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
    if [ $? -ne 0 ]; then
        echo "Failed to create build directory: $BUILD_DIR" >&2
        exit 1
    fi
fi

# Navigate to build directory
cd "$BUILD_DIR"

# Generate build files with CMake
echo "Generating build files with CMake..."
cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed" >&2
    exit 1
fi

# Build the project
echo "Building setproxy..."
cmake --build .
if [ $? -ne 0 ]; then
    echo "Build failed" >&2
    exit 1
fi

# Install if requested
if [ -n "$INSTALL" ]; then
    echo "Installing setproxy (requires sudo)..."
    sudo cmake --install .
    if [ $? -ne 0 ]; then
        echo "Installation failed" >&2
        exit 1
    fi
    echo "Installation completed successfully."
fi

echo "Build completed successfully!"
echo "Binary location: $BUILD_DIR/setproxy"

# Return to original directory
cd ..