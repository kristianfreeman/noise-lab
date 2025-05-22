# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Noise Lab is a Max for Live device designed for EDM and techno producers to create noise effects for risers, impacts, transitions, and textures. The project uses Max/MSP to create a noise generator with multiple algorithms and controls.

## Technical Architecture

### Core Components

1. **Noise Generation** - Multiple noise algorithms (White, Pink, Brown, Digital, Analog Simulation)
2. **Trigger System** - Free Run, MIDI Trigger, Host Sync, and One-Shot modes
3. **Envelope Section** - Attack, Decay, Sustain, Release parameters
4. **Modulation System** - LFO with rate, sync, depth, and target selection
5. **Filter Section** - Filter type (LP/BP/HP), cutoff, resonance, and filter envelope
6. **Effects Processing** - Drive, Bitcrush, Stereo Width
7. **Output Stage** - Level control and Dry/Wet mix

### Signal Flow
Noise Source → Pre-Filter Drive → Filter Section → Effects Processing → Amplitude Envelope → Output Stage

### Max/MSP Implementation
- Noise generation: `noise~`, `pink~`, custom MSP code
- Filtering: `biquad~` with coefficient calculation
- Envelopes: `function` objects with visual feedback
- Modulation: `phasor~` with scaling and mapping
- Effects: `overdrive~`, `degrade~`, custom stereo processing

## Commands

### Project Setup
```bash
# Install dependencies
npm install
```

### Development
The project is primarily developed in Max/MSP. Open the noise-lab.maxproj file in Max 8 to work on the device.

## Max for Live Integration
- The device is designed to work within Ableton Live's environment
- Parameters are exposed for automation and modulation in Live
- Compatible with Push controllers
- Supports clip envelope control

## Project Roadmap
Future features being considered include:
- Noise sequencer for rhythmic patterns
- Multiple parallel noise layers
- Advanced modulation matrix
- Sample import functionality
- MIDI CC mapping
- Spectral processing tools

## Development Guidelines
- When implementing anything in the Max device, use idiomatic Max code and design principles.