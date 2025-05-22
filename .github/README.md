# GitHub Actions CI/CD

This directory contains GitHub Actions workflows for automated building and testing of the Noise Lab plugin.

## Workflows

### `build.yml` - Main Build Workflow

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` branch
- Git tags starting with `v` (for releases)

**Build Matrix:**
- **macOS**: Builds VST3, AU, and Standalone formats
- **Windows**: Builds VST3 and Standalone formats  
- **Linux**: Builds VST3 and Standalone formats

**Artifacts:**
- Downloadable plugin binaries for each platform
- 30-day retention for development builds
- Permanent releases for tagged versions

### Dependencies Installed

**macOS:**
- JUCE 8.0.7 framework
- CMake 3.22+
- Xcode command line tools (pre-installed)

**Windows:**
- JUCE 8.0.7 framework
- CMake 3.22+
- Visual Studio 2022 Build Tools
- MSBuild

**Linux:**
- JUCE 8.0.7 framework
- CMake 3.22+
- Audio development libraries:
  - ALSA (`libasound2-dev`)
  - JACK (`libjack-jackd2-dev`) 
  - LADSPA SDK
  - WebKit2GTK for web browser support
  - X11 libraries for GUI
  - Mesa libraries for OpenGL

## Release Process

1. **Development**: Push commits to `main` or `develop` - triggers build artifacts
2. **Release**: Create and push a git tag starting with `v` (e.g., `v1.0.0`)
3. **Distribution**: GitHub automatically creates a release with downloadable plugin binaries

### Creating a Release

```bash
# Tag a release
git tag v1.0.0
git push origin v1.0.0

# This will trigger the release workflow and create:
# - noise-lab-macos.zip (VST3, AU, Standalone)
# - noise-lab-windows.zip (VST3, Standalone)  
# - noise-lab-linux.zip (VST3, Standalone)
```

## Build Artifacts

Each successful build creates downloadable artifacts containing:

**macOS (`noise-lab-macos`):**
- `Noise Lab.vst3` - VST3 plugin
- `Noise Lab.component` - AU plugin
- `Noise Lab.app` - Standalone application

**Windows (`noise-lab-windows`):**
- `Noise Lab.vst3` - VST3 plugin
- `Noise Lab.exe` - Standalone application

**Linux (`noise-lab-linux`):**
- `Noise Lab.vst3` - VST3 plugin
- `Noise Lab` - Standalone executable

## Installation Instructions

**macOS:**
```bash
# VST3: Copy to ~/Library/Audio/Plug-Ins/VST3/
# AU: Copy to ~/Library/Audio/Plug-Ins/Components/
# Standalone: Copy to /Applications/ or run directly
```

**Windows:**
```bash
# VST3: Copy to %COMMONPROGRAMFILES%\VST3\
# Standalone: Run .exe directly or copy to desired location
```

**Linux:**
```bash
# VST3: Copy to ~/.vst3/ or /usr/lib/vst3/
# Standalone: Make executable and run or copy to /usr/local/bin/
```

## Troubleshooting

**Build Failures:**
- Check the Actions tab on GitHub for detailed logs
- Common issues: JUCE download failures, missing dependencies
- Platform-specific issues are isolated to their respective jobs

**Missing Artifacts:**
- Ensure the build completed successfully
- Check that the artifact paths match the expected JUCE output directories
- Verify the plugin targets built without errors

**Local Testing:**
You can test the same build process locally:
```bash
# macOS/Linux
make clean && make

# Windows (with JUCE installed)
mkdir build && cd build
cmake .. -DJUCE_PATH=C:/JUCE -G "Visual Studio 17 2022"
cmake --build . --config Release
```