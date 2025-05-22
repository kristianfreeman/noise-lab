# Noise Lab

A versatile VST/AU plugin designed specifically for EDM and techno producers who need quick access to high-quality noise effects for creating risers, impacts, transitions, and textural elements.

## Features

### Noise Sources
- **White Noise** - Full-spectrum random noise, the classic foundation
- **Pink Noise** - Warmer, with reduced high frequencies (1/f spectrum)
- **Brown Noise** - Even more low-end focused, great for rumbles (1/f² spectrum)
- **Digital Crunch** - Bit-crushed, glitchy digital noise with aliasing artifacts
- **Analog Simulation** - Emulated transistor/circuit noise with subtle warmth

### Trigger Modes
- **Free Run** - Continuous noise generation
- **MIDI Trigger** - Activates noise on MIDI note input
- **Host Sync** - Syncs to DAW's transport for beat-aligned effects
- **One-Shot** - Plays a single envelope cycle then stops

### Main Controls

#### Envelope Section
- **Attack** (1ms - 10s): Controls the time it takes for noise to reach full volume
- **Decay** (1ms - 30s): Sets how quickly the noise fades after reaching peak
- **Sustain** (0-100%): Level maintained while trigger is held (in MIDI mode)
- **Release** (1ms - 30s): Fade-out time after trigger ends

#### Modulation Section
- **Rate** (0.1Hz - 50Hz): Speed of internal LFO
- **Sync Toggle**: Syncs LFO to host tempo when enabled
- **Depth** (0-100%): Amount of LFO modulation applied
- **Target Selector**: Assigns LFO to Volume, Filter Cutoff, Filter Resonance, or Pitch/Rate

#### Filter Section
- **Filter Type**: LP/BP/HP (Low-pass, Band-pass, High-pass)
- **Cutoff** (20Hz - 20kHz): Filter cutoff frequency
- **Resonance** (0-100%): Filter resonance/Q amount

#### Effects Section
- **Drive** (0-100%): Adds harmonic saturation and compression
- **Bitcrush** (1-16 bit): Reduces bit depth for digital artifacts
- **Stereo Width** (0-200%): Controls the stereo image from mono to super-wide

### Global Controls
- **Output Level** (-inf to +6dB): Master volume with visual feedback
- **Dry/Wet** (0-100%): Blend between processed and clean signal

## Development

This project uses the JUCE framework to create VST3 and AU plugins. The implementation follows the design document, providing a powerful noise generator with extensive modulation and filtering capabilities.

### Project Structure
- `/src` - Source code files
- `/assets` - UI graphics and resources
- `/build` - Build output directory

### Building the Project
```bash
# Configure and build
make

# Install the plugin
make install

# Clean the build directory
make clean
```

### Build Status

The current build has compatibility issues with macOS 15.4 and JUCE 7.0.11 (from Nix). This is due to deprecated functions in macOS 15.4 that JUCE 7.0.11 relies on.

#### Potential Solutions

1. **Use a newer version of JUCE** - JUCE 7.0.11 from Nix is not compatible with macOS 15.4. Upgrading to a newer version of JUCE should resolve these issues.

2. **Use a different macOS version** - If using an older version of macOS is an option, JUCE 7.0.11 may work correctly.

3. **Use a different build system** - The current issues are related to the JUCE build system and its compatibility with macOS 15.4.

### Next Steps

To make the plugin fully compatible with current macOS versions, we recommend:

1. Updating to JUCE 7.0.12 or newer
2. Using the latest CMake build system for JUCE
3. Testing with a minimal build first to ensure compatibility

See the [BUILD.md](BUILD.md) file for detailed build instructions and compatibility information.

## Technical Implementation

### Signal Flow
1. Noise Source → 
2. Pre-Filter Drive → 
3. Filter Section → 
4. Effects Processing → 
5. Amplitude Envelope → 
6. Output Stage

## License

ISC License