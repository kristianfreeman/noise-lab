# Building Noise Lab VST/AU Plugin

This document provides instructions for building the Noise Lab plugin from source code.

## Prerequisites

1. **JUCE Framework**: 
   - JUCE 7.0.11 or newer is required
   - For Nix users, JUCE is available in the Nix store

2. **C++ Development Environment**:
   - **macOS**: Xcode 10.0 or later
   - **Windows**: Visual Studio 2019 or later
   - **Linux**: GCC 7.0 or later

3. **CMake**: Version 3.15 or later

## Compatibility Issues

### macOS 15 Compatibility ✅ RESOLVED

**Status**: Build now works successfully with JUCE 8.0.7 from `/Applications/JUCE`.

JUCE 7.0.11 (provided by Nix) has compatibility issues with macOS 15 due to deprecated APIs:

1. **Primary Error**: `CGWindowListCreateImage` is unavailable in macOS 15, with the message:
   "obsoleted in macOS 15.0 - Please use ScreenCaptureKit instead."

2. **Additional Deprecated Functions**:
   - `CVDisplayLinkGetNominalOutputVideoRefreshPeriod`
   - `CVDisplayLinkRelease`
   - `CVDisplayLinkCreateWithCGDisplay`
   - `CVDisplayLinkSetOutputCallback`
   - `CVDisplayLinkStart`
   - `CVDisplayLinkStop`

### Solutions

#### ✅ Working Solution: Use JUCE 8.0.7
The project now successfully builds with JUCE 8.0.7 from `/Applications/JUCE`:

```bash
# Build with the newer JUCE version
make

# The project automatically uses /Applications/JUCE
# If you have JUCE elsewhere, you can specify:
make JUCE_PATH=/path/to/your/JUCE
```

**Build Results:**
- ✅ VST3 plugin built successfully
- ✅ Automatically installed to `~/Library/Audio/Plug-Ins/VST3/`
- ✅ No macOS 15 compatibility issues

#### ⚠️ Attempted Workarounds (Failed)
We attempted several workarounds that did not resolve the issue:

1. **Aggressive warning suppression**: Added `-w`, `-Wno-error`, `-Wno-deprecated-declarations`
2. **Headless build**: Used `-DHEADLESS_BUILD=ON` to exclude GUI components
3. **Conditional compilation**: Modified source with `#ifndef JUCE_HEADLESS`
4. **CMake exclusions**: Removed GUI libraries from link targets
5. **Deployment target**: Set macOS deployment target to 10.15

None of these worked because JUCE's plugin framework still compiles core GUI modules even in headless mode.

#### 🔄 Alternative Solutions
1. **Use macOS 14 or earlier**: The deprecated APIs are still available
2. **Use Linux or Windows**: The issue is specific to macOS 15
3. **Wait for Nix package update**: The Nix JUCE package may be updated to a compatible version

## Building with Make (Recommended)

We provide a simplified Makefile that wraps the CMake commands:

```bash
# Configure and build
make

# Install the plugin
make install

# Clean the build directory
make clean

# Run the standalone version (if built)
make run
```

You can specify a custom JUCE path:

```bash
make JUCE_PATH=/path/to/your/JUCE
```

## Building with CMake Directly

If you prefer to use CMake directly:

1. **Clone or download the repository**:
   ```bash
   git clone <repository-url>
   cd noise-lab
   ```

2. **Create a build directory**:
   ```bash
   mkdir -p build
   cd build
   ```

3. **Configure with CMake**:
   ```bash
   cmake .. -DJUCE_PATH=/path/to/your/JUCE
   ```
   Replace `/path/to/your/JUCE` with the actual path to your JUCE installation.

4. **Build the plugin**:
   ```bash
   cmake --build . --config Release
   ```

5. **Install the plugin** (optional):
   ```bash
   cmake --install .
   ```

## Building with Projucer

Alternatively, you can use JUCE's Projucer application:

1. **Open Projucer** and create a new Audio Plugin project

2. **Add the source files** from the `src` directory

3. **Configure the plugin settings** according to your preferences

4. **Save and open** in your preferred IDE

5. **Build the plugin** using your IDE's build system

## Plugin Installation Locations

After building, you can manually install the plugin in the standard locations:

### VST3 Plugins

- **macOS**: `/Library/Audio/Plug-Ins/VST3` or `~/Library/Audio/Plug-Ins/VST3`
- **Windows**: `C:\Program Files\Common Files\VST3`
- **Linux**: `/usr/lib/vst3` or `~/.vst3`

### AU Plugins (macOS only)

- `/Library/Audio/Plug-Ins/Components` or `~/Library/Audio/Plug-Ins/Components`

## Troubleshooting

- **JUCE path not found**: Ensure you're providing the correct path to your JUCE installation.
- **Build errors**: Make sure you have the latest version of JUCE and your development environment.
- **Plugin not loading**: Check that the plugin is built for the correct architecture and format supported by your DAW.
- **macOS compatibility issues**: See the compatibility section above for known issues with macOS 15.4.

## Next Steps for Development

To ensure compatibility with current operating systems:

1. **Update JUCE**: 
   - Use JUCE 7.0.12 or newer which should fix the macOS 15.4 compatibility issues
   - Consider using JUCE's CMake integration directly rather than via Nix

2. **Create a Minimal Test Build**:
   - Create a minimal JUCE plugin to verify that the build system works correctly
   - Once confirmed working, integrate the full Noise Lab functionality

3. **Improve Build System**:
   - Add better error handling to the Makefile
   - Create a headless build option that actually excludes GUI modules
   - Add cross-platform testing

## Getting Help

If you encounter any issues, please check the JUCE forum or open an issue in the repository.