# FionaGain

A minimal gain plugin for macOS, built with [JUCE](https://juce.com) and CMake. It's a just-for-fun project and a starting point for more ambitious plugins.

- **Formats:** VST3 and Audio Unit (AU)
- **One parameter:** Gain, from −60 dB to +12 dB (default 0 dB), smoothed so it doesn't click when you move it
- **Channels:** mono or stereo
- **UI:** JUCE's built-in generic editor (a plain slider) for now

## Requirements

- macOS 11 or later (built and tested on Apple Silicon)
- Xcode Command Line Tools: `xcode-select --install`. Full Xcode isn't needed.
- CMake 3.22 or later, and Ninja: `brew install cmake ninja`

JUCE doesn't need to be installed separately. CMake downloads a pinned release (currently 9.0.3) the first time you configure.

## Build

```sh
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

The first configure takes a minute or so while JUCE downloads. For an optimised build, use `-DCMAKE_BUILD_TYPE=Release` with a separate build folder, such as `-B build-release`.

## Install

Every build copies the plugins to where macOS hosts look for them:

| Format | Location |
|---|---|
| VST3 | `~/Library/Audio/Plug-Ins/VST3/FionaGain.vst3` |
| AU | `~/Library/Audio/Plug-Ins/Components/FionaGain.component` |

In **Reaper**, go to *Preferences → Plug-ins → VST → Re-scan*, then add **FionaGain** to a track.

## Validate

The plugin passes [pluginval](https://github.com/Tracktion/pluginval) at strictness level 5 and Apple's `auval`:

```sh
pluginval --strictness-level 5 --validate ~/Library/Audio/Plug-Ins/VST3/FionaGain.vst3
auval -v aufx FGan Fiwi
```

pluginval isn't included in this repo. Download the macOS build from its [releases page](https://github.com/Tracktion/pluginval/releases).

## Project layout

```
CMakeLists.txt            Build setup: fetches JUCE, defines the plugin target
Source/PluginProcessor.*  The audio processor: parameter, gain processing, state save/restore
CLAUDE.md                 Working notes and rules for development (including real-time safety)
```

## Troubleshooting

**`fatal error: 'utility' file not found` during configure.** This comes from an old Command Line Tools install leaving a stale `/Library/Developer/CommandLineTools/usr/include/c++` folder behind, which hides the real C++ headers. Move that folder out of the way, for example:

```sh
sudo mv /Library/Developer/CommandLineTools/usr/include/c++ ~/Desktop/stale-clt-c++-backup
```

Or reinstall the Command Line Tools.
