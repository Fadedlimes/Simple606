# Simple 606

An intuitive, fully synthesized analog drum machine and groovebox plugin built with JUCE. Available as a **VST3 plugin** and **Standalone application** for Linux, Windows, and macOS.

<img width="963" height="419" alt="image" src="https://github.com/user-attachments/assets/3366a1ae-b6ce-4716-b28c-6a1d77781f98" />
<img width="958" height="417" alt="image" src="https://github.com/user-attachments/assets/6be97048-2553-4631-9785-b9aa9da67896" />
<img width="960" height="420" alt="image" src="https://github.com/user-attachments/assets/bd6136fe-e537-4133-81ab-b07bd8c03908" />

⚠️ Note: This project was originally built for Linux and with Linux in mind, so please note the Mac and Windows builds are entirely untested.
---

## 📖 About This Project

**Simple 606** is a standalone application and VST3 plugin wrapper built around the open-source DSP code from Matthew Fecher's ([@AnalogMatthew](https://github.com/analogcode)) [606-Inspired-Synth-Drums](https://github.com/analogcode/606-Inspired-Synth-Drums). 

### Why was this made?
Modern Digital Audio Workstations (DAWs) and software synthesizers are often overloaded with complex routing matrices, deep menus, and steep learning curves. This project was born out of a desire to make electronic music-making immediately accessible, tactile, and fun for anyone—especially for people like my partner, for whom a traditional DAW is simply too overwhelming. This project was made with intimidate hands on fun as a core principle.

### Transparency & AI Disclosure
In the interest of open disclosure, the JUCE wrapper, UI layout, sequencer, mixer, effects engine, and cross-platform build systems were assembled and developed with the assistance of **Gemini/Gemma**.

---

## ✨ Features

### 🥁 1. Analog Drum Synthesis Engine (`DRUMS`)
- **100% Pure Synthesis:** Contains zero samples. Every drum sound is calculated mathematically in real-time.
- **8 Classic Drum Voices:** Bass Drum, Snare Drum, Hand Clap, Closed Hi-Hat, Open Hi-Hat, Cymbal, Low Tom, and High Tom.
- **608 XL Sub Mode:** A dedicated switch on the Kick Drum that opens up the decay into a deep, booming 808-style sub-bass.
- **Kick Heat:** Dial in subtle analog saturation specifically on the Bass Drum body.
- **Per-Voice Stereo Panning:** Independent L/R stereo positioning for each drum voice.

### 🎮 2. Interactive Drum Pads (`PADS`)
- 8 large, responsive velocity-sensitive drum pads.
- **Computer Keyboard Support:** Play beats directly using your computer keyboard without needing a MIDI controller:
  - **Home Row:** `A` (BD), `S` (SN), `D` (CL), `F` (CH), `G` (OH), `Y`(CY) `H` (LT), `J` (HT),
  - **Number Row:** `1`, `2`, `3`, `4`, `5`, `6`, `7`
  - **Bottom Row:** `Z`, `X`, `C`, `V`, `B`, `N`, `M`
  - **Spacebar:** Start / Stop Sequencer playback

### ⏱️ 3. 64-Step XOX Sequencer (`SEQUENCER`)
- **4 Pages (Up to 64 Steps):** Seamlessly navigate and program across Pages `1`, `2`, `3`, and `4`.
- **Dynamic Accent Track (`ACC`):** Authentic vintage-style accent track delivering a $+5.5\text{ dB}$ punch and dynamic envelope modulation on accented hits.
- **Page Copy & Paste:** Copy any 16-step pattern across all 8 tracks and paste it to other pages with a single click.
- **Per-Track Mute & Solo:** Dedicated `M` and `S` buttons beside each lane (synchronized with the mixer).
- **DAW Sync or Internal Clock:** Automatically locks to your DAW host tempo or runs on internal BPM.

### 🎚️ 4. Console Mixer (`MIXER`)
- 8 individual voice channel strips + Master Output channel.
- Vintage-style aluminum vertical faders and pan dials.
- Isolated channel **Mute** and **Solo** routing logic.

### 🎛️ 5. Modular FX Rack (`FX RACK`)
- **3 × 8 FX Routing Matrix:** Route any individual drum voice to specific effects independently (e.g. keeping the kick dry while sending hats and claps through delay and reverb).
- **Analog Warmth Overdrive:** 2nd/3rd harmonic tube saturation with warm low-mid tone shaping.
- **Musical Stereo Delay:** Stepped rhythmic subdivisions (`1/32` to `1/2`) with true stereo **Ping-Pong** bouncing.
- **Dattorro Plate Reverb:** 4-stage all-pass diffused reverb with selectable **`ROOM`** (tight & punchy) and **`HALL`** (lush & spacious) modes.
- **Master Resonant Filter:** Zero-delay state variable filter with continuous smooth sweeps, **`LP / HP`** modes, and **`12 / 24 dB`** slope switches.

### ⚙️ 6. Settings & Customization (`SETTINGS`)
- **9 UI Color Themes:** `Neon Violet` (Default), `Electric Blue`, `TR Red`, `Sunset Yellow`, `Amber CRT`, `Acid Green`, `Cyber Teal`, `Hot Pink`, and `Trans Pride`.
- **Trans Pride Easter Egg:** Custom gradient styling for the Kick Drum's `TRANS` parameter.
- **12-Bit Vintage Sampler Engine:** Instant $26.04\text{ kHz}$ E-mu SP-1200 clock aliasing and DAC crunch.
- **Shareable Preset Files:** Save and load your entire drum machine state and 64-step sequence into human-readable `.simple606` preset files.

---

## 📥 Installation & Download

Pre-compiled binaries for **Linux**, **Windows**, and **macOS** are automatically generated via GitHub Actions:

1. Head to the **[Releases](https://github.com/Fadedlimes/Simple606/releases)** section (or the **[Actions](https://github.com/Fadedlimes/Simple606/actions)** tab for latest builds).
2. Download the package for your operating system:
   - 🪟 **Windows:** Extract and copy `Simple 606.vst3` into `C:\Program Files\Common Files\VST3\` (or run `Simple 606.exe`).
   - 🍎 **macOS:** Copy `Simple 606.vst3` into `/Library/Audio/Plug-Ins/VST3/` (or run `Simple 606.app`).
   - 🐧 **Linux:** Copy `Simple 606.vst3` into `~/.vst3/` (or run `./Simple\ 606`).

---

## 🛠️ Building from Source

### Prerequisites
- CMake 3.15 or newer
- C++14 compatible compiler (GCC, Clang, or MSVC)
- Standard audio/X11 development libraries (on Linux)

### Build Commands
```bash
# Clone the repository
git clone https://github.com/Fadedlimes/Simple606.git
cd Simple606

# Configure & build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
