# Noise Lab: Max for Live Noise Generator

## Design Document

### Overview
Noise Lab is a versatile Max for Live device designed specifically for EDM and techno producers who need quick access to high-quality noise effects for creating risers, impacts, transitions, and textural elements. It combines multiple noise generation algorithms with intuitive controls and performance-focused features to seamlessly integrate into any production workflow.

### Core Philosophy
As both a programmer and techno producer, I've found that the most useful noise tools strike a balance between flexibility and immediacy. Noise Lab aims to provide enough sound design depth to be creative while remaining straightforward enough to use in the heat of production or live performance.

## Features

### Noise Sources
The device will offer five distinct noise algorithms, each with its own character:

1. **White Noise** - Full-spectrum random noise, the classic foundation
2. **Pink Noise** - Warmer, with reduced high frequencies (1/f spectrum)
3. **Brown Noise** - Even more low-end focused, great for rumbles (1/f² spectrum)
4. **Digital Crunch** - Bit-crushed, glitchy digital noise with aliasing artifacts
5. **Analog Simulation** - Emulated transistor/circuit noise with subtle warmth

### Trigger Modes
1. **Free Run** - Continuous noise generation
2. **MIDI Trigger** - Activates noise on MIDI note input
3. **Host Sync** - Syncs to Ableton's transport for beat-aligned effects
4. **One-Shot** - Plays a single envelope cycle then stops

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
- **Target Selector**: Assigns LFO to:
  - Volume
  - Filter Cutoff
  - Filter Resonance
  - Pitch/Rate (for certain noise types)

#### Filter Section
- **Filter Type**: LP/BP/HP (Low-pass, Band-pass, High-pass)
- **Cutoff** (20Hz - 20kHz): Filter cutoff frequency
- **Resonance** (0-100%): Filter resonance/Q amount
- **Cutoff Envelope**: Dedicated envelope for filter movement:
  - Amount (-100% to +100%): Intensity and direction of envelope
  - Attack (1ms - 10s): Time to reach full modulation
  - Decay (1ms - 30s): Time to return to base value

#### Effects Section
- **Drive** (0-100%): Adds harmonic saturation and compression
- **Bitcrush** (1-16 bit): Reduces bit depth for digital artifacts
- **Stereo Width** (0-200%): Controls the stereo image from mono to super-wide

### Global Controls
- **Output Level** (-inf to +6dB): Master volume with visual feedback
- **Dry/Wet** (0-100%): Blend between processed and clean signal

## Presets

The device will ship with 24 carefully designed presets across four categories:

### Risers (6 presets)
1. **Classic Buildup** - Gradual filtered white noise rise
2. **Cybernetic Tension** - Digital, glitchy ascension with time-synced modulation
3. **Harmonic Stacker** - Resonant peaks that build in layers
4. **Sub Rumble Rise** - Brown noise with growing low-end presence
5. **Phase Cannon** - Wide stereo with phasing effects that intensify
6. **Granular Climb** - Textured, particle-like ascension

### Impacts & Drops (6 presets)
1. **Megaton Hit** - Explosive, compressed burst with fast attack
2. **Digital Destruction** - Bitcrushed impact with artifacts
3. **Subterranean Impact** - Low-end focused hit with physical feeling
4. **Scatter Bomb** - Dispersing noise with stereo spread
5. **Circuit Breaker** - Electronic discharge sound with analog character
6. **Vacuum Drop** - Creates a "sucking" effect before the impact

### Textures (6 presets)
1. **Machine Room** - Industrial, consistent background noise
2. **Vinyl Crackle** - Warm, nostalgic surface noise
3. **Digital Rain** - Scattered, granular texture like electronic precipitation
4. **Ocean Floor** - Deep, filtered pressure waves
5. **Static Field** - Electrical interference patterns
6. **Radiation Waves** - Pulsing, modulated noise bed

### FX Elements (6 presets)
1. **Snare Enhancer** - Adds noise to layer with drum hits
2. **Transition Sweep** - Quick filtered movement for scene changes
3. **Glitch Scatter** - Randomized bursts of digital artifacts
4. **Laser Zaps** - Sharp, quick noise bursts with filter resonance
5. **Radio Tuning** - Simulates scanning between stations
6. **Air Pump** - Rhythmic noise pulses with compression

## Technical Implementation

### Signal Flow
1. Noise Source → 
2. Pre-Filter Drive → 
3. Filter Section → 
4. Effects Processing → 
5. Amplitude Envelope → 
6. Output Stage

### Max/MSP Objects
- Noise generation: `noise~`, `pink~`, custom MSP code for specialized types
- Filtering: `biquad~` with coefficient calculation for different filter types
- Envelopes: `function` objects with curve drawing for visual feedback
- Modulation: `phasor~` feeding into scaling and mapping objects
- Effects: Combination of `overdrive~`, `degrade~`, custom stereo processing

### CPU Optimization
- Dynamic DSP patching to disable unused modules
- Optional quality settings for CPU-intensive effects
- Efficient use of `poly~` for polyphonic versions of certain effects

### UI Design
- Clean, focused interface with dark theme for studio visibility
- Color-coding for different functional sections
- Visual feedback showing noise activity, filter curves, and envelope shapes
- Collapsible advanced sections to avoid overwhelming new users

## Extended Features (v2 Roadmap)

For future updates, these features are being considered:
- Noise sequencer for rhythmic patterns
- Multiple parallel noise layers with crossfading
- Advanced modulation matrix with multiple LFOs
- Sample import to create hybrid noise sources
- MIDI CC mapping for all parameters with learn function
- Spectral processing tools for more detailed sound shaping

## Integration Points

### Ableton Live Specific Features
- Parameter automation recording directly in Live's automation lanes
- Seamless audio routing within Ableton's signal flow
- Clip envelope control of key parameters
- Compatible with Push controllers for hands-on interaction
- Support for Live's modulation capabilities

### Performance Considerations
- Minimal latency for real-time performance
- MIDI mappable macro controls for live tweaking
- CPU usage indicator to manage performance impact
- Preset morphing capabilities for smooth transitions
