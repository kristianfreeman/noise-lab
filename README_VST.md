# Simple Noise Generator VST

This project demonstrates the core algorithm for a simple noise generator that could be used in a VST/AU plugin. To turn this into a working VST plugin, you would need to integrate it with a VST framework like JUCE.

## The Algorithm

The `SimpleNoiseGenerator.cpp` file contains a complete implementation of three types of noise:

1. **White Noise** - Random values with equal energy at all frequencies
2. **Pink Noise** - Noise with energy inversely proportional to frequency (1/f)
3. **Brown Noise** - Noise with energy decreasing more rapidly at higher frequencies (1/f²)

It also includes a simple low-pass filter implementation to shape the noise spectrum.

## Building a Complete VST/AU Plugin

To build a complete VST/AU plugin, follow these steps:

1. **Install JUCE Framework**:
   - Download JUCE from [juce.com](https://juce.com/get-juce/)
   - Install it on your system

2. **Create a New Plugin Project**:
   - Open Projucer (JUCE's project manager)
   - Create a new Audio Plugin project
   - Configure it for VST3, AU, or other formats

3. **Integrate the Noise Generator**:
   - Copy the noise generator classes from `SimpleNoiseGenerator.cpp` into your plugin's processor class
   - Set up the necessary audio processing in your plugin's `processBlock` method
   - Create parameters for noise type, gain, and filter settings

4. **Create a User Interface**:
   - Design a simple UI with controls for:
     - Noise type selection
     - Gain/volume
     - Filter cutoff
     - Any additional parameters

5. **Build and Export**:
   - Build your plugin through JUCE
   - Export it as VST3/AU/etc.
   - Install it in your DAW's plugin folder

## Alternative: Use a No-Code VST Creator

If you don't want to code, consider using a no-code VST creator like:

- [Tone2 Icarus](https://www.tone2.com/icarus.html)
- [SynthEdit](http://www.synthedit.com/)
- [Kilohearts Snap Heap](https://kilohearts.com/products/snap_heap)

These tools allow you to create VST plugins visually, similar to Max/MSP but with direct export to VST format.

## Resources for VST Development

- [JUCE Tutorials](https://juce.com/learn/tutorials/)
- [The Audio Programmer YouTube Channel](https://www.youtube.com/theaudioprogrammer)
- [KVR Audio Developer Forum](https://www.kvraudio.com/forum/viewforum.php?f=33)

## Next Steps

If you want to take this project further:
1. Integrate with JUCE
2. Add more noise types
3. Add more sophisticated filters
4. Create a nice-looking UI
5. Add modulation options (LFO, envelope followers)