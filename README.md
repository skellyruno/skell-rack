# Modular FX Suite VST3 Plugin

An interactive C++/JUCE modular signal rack featuring:
- **Dark Reverser**: Purple radial arc Mix knob and beat division selector.
- **Noise Gate**: Dynamic dark waveform display with interactive draggable Threshold pill and analog Attack/Release controls.
- **Chorus Ensemble**: Slate-blue layout with Japanese Seigaiha vector wave graphics and white teardrop dials.

## Build Instructions

### Prerequisites
- **CMake** 3.22+
- **C++17 Compiler** (MSVC 2022 on Windows, Xcode on macOS, or GCC/Clang on Linux)

### Building via Terminal
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release