# FionaGain

A minimal JUCE audio plugin (VST3 + AU, macOS) built with CMake. One parameter: gain in dB (−60 to +12, default 0), using JUCE's generic editor.

## 🚨 Hard rule: real-time safety in `processBlock`

`processBlock` runs on the host's real-time audio thread. Nothing inside it, or in anything it calls, may:

- **Allocate or free memory**: no `new`/`delete`, `malloc`, `std::make_unique`, `std::vector::push_back`/`resize`, building a `juce::String`, or creating an `AudioBuffer`. Allocate up front in `prepareToPlay` or the constructor.
- **Take locks**: no `std::mutex`, `juce::CriticalSection`, `ScopedLock`, or waiting on anything. Share data with other threads through `std::atomic` or lock-free FIFOs (`juce::AbstractFifo`).
- **Log**: no `DBG`, `juce::Logger`, `std::cout` or `printf`.
- **Do any I/O**: no file, network, or console access, and no message-thread calls.

Parameters are read through cached `std::atomic<float>*` pointers (from `apvts.getRawParameterValue`). Never look a parameter up by its string ID inside `processBlock`.

## Build

Prerequisites: Xcode Command Line Tools, CMake ≥ 3.22, Ninja (`brew install cmake ninja`). Full Xcode is not needed.

```sh
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug   # configure (first run downloads JUCE)
cmake --build build                                 # build
```

For a Release build, use `-DCMAKE_BUILD_TYPE=Release` in a separate build dir, for example `-B build-release`.

## Where outputs land

- Build artefacts: `build/FionaGain_artefacts/Debug/VST3/FionaGain.vst3` and `build/FionaGain_artefacts/Debug/AU/FionaGain.component`
- After each build they're copied (via `COPY_PLUGIN_AFTER_BUILD`) to:
  - `~/Library/Audio/Plug-Ins/VST3/FionaGain.vst3`
  - `~/Library/Audio/Plug-Ins/Components/FionaGain.component`
- In Reaper, go to Preferences → Plug-ins → VST → Re-scan to pick up changes.

## Validation

- **pluginval** (not committed; download the macOS zip from https://github.com/Tracktion/pluginval/releases):
  `pluginval --strictness-level 5 --validate ~/Library/Audio/Plug-Ins/VST3/FionaGain.vst3`
- **auval**: `auval -v aufx FGan Fiwi`. Plugin code `FGan` and manufacturer code `Fiwi` are set in `CMakeLists.txt`.

## Project layout and conventions

- `CMakeLists.txt`: JUCE is fetched with FetchContent, pinned to a **release tag** (currently `9.0.3`). Bump it on purpose by changing `GIT_TAG`. Never track a branch.
- `Source/PluginProcessor.{h,cpp}`: the processor, the APVTS parameter layout, and state save/restore.
- **Never commit build output.** This repo syncs between two Macs via GitHub. `build/`, `cmake-build-*/` and editor files are in `.gitignore`.
- Commit after each working step, with a clear message, so the git log tells the story of the build.

## Known machine gotcha

If configure fails with `fatal error: 'utility' file not found`, check for a stale `/Library/Developer/CommandLineTools/usr/include/c++` folder left behind by an old Command Line Tools install. It hides the SDK's C++ headers. Move it aside (`sudo mv ... ~/Desktop/...`) or reinstall the Command Line Tools. Don't work around it in `CMakeLists.txt`.
