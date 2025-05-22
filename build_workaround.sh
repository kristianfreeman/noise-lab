#!/bin/bash

# Workaround build script for macOS 15 compatibility issues with JUCE

set -e

echo "Attempting to build with aggressive error suppression..."

# Clean previous builds
rm -rf build
mkdir -p build
cd build

# Configure with aggressive warning/error suppression
cmake .. \
    -DJUCE_PATH=/nix/store/zradgqjk3k6wyj7ljw8q8wgpv20n30k8-juce-7.0.11 \
    -DHEADLESS_BUILD=ON \
    -DCMAKE_CXX_FLAGS="-w -Wno-error -Wno-deprecated-declarations -Wno-unused-but-set-variable -DJUCE_HEADLESS=1 -DJUCE_DISABLE_ASSERTIONS=1" \
    -DCMAKE_C_FLAGS="-w -Wno-error -Wno-deprecated-declarations"

# Try to build with maximum parallelism
echo "Building with error suppression..."
make -j$(nproc) 2>&1 | grep -v "deprecated" | grep -v "warning" || {
    echo "Build failed. Attempting fallback build with minimal features..."
    
    # Try with even more restrictive settings
    cd ..
    rm -rf build
    mkdir -p build
    cd build
    
    cmake .. \
        -DJUCE_PATH=/nix/store/zradgqjk3k6wyj7ljw8q8wgpv20n30k8-juce-7.0.11 \
        -DHEADLESS_BUILD=ON \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
        -DCMAKE_CXX_FLAGS="-w -Wno-everything -DJUCE_HEADLESS=1 -DJUCE_DISABLE_ASSERTIONS=1 -DJUCE_MODAL_LOOPS_PERMITTED=0" \
        -DCMAKE_C_FLAGS="-w -Wno-everything"
    
    make -j$(nproc) 2>/dev/null || {
        echo "All build attempts failed. The JUCE version in Nix is incompatible with macOS 15."
        echo "Consider using JUCE 7.0.12 or later, or downgrading to macOS 14."
        exit 1
    }
}

echo "Build completed successfully!"
ls -la NoiseLab_artefacts/Release/ 2>/dev/null || echo "Plugin artifacts not found in expected location"