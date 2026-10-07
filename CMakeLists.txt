cmake_minimum_required(VERSION 3.22)

# Must be set before project() to take effect.
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")

project(FionaGain VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- JUCE (pinned release tag; bump deliberately) ---------------------------
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        9.0.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ---- Plugin -----------------------------------------------------------------
juce_add_plugin(FionaGain
    COMPANY_NAME              "Fiona Wiggins"
    PRODUCT_NAME              "FionaGain"
    PLUGIN_MANUFACTURER_CODE  Fiwi
    PLUGIN_CODE               FGan
    FORMATS                   VST3 AU
    IS_SYNTH                  FALSE
    NEEDS_MIDI_INPUT          FALSE
    NEEDS_MIDI_OUTPUT         FALSE
    IS_MIDI_EFFECT            FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    # Copies VST3 -> ~/Library/Audio/Plug-Ins/VST3 and
    #        AU   -> ~/Library/Audio/Plug-Ins/Components after each build.
    COPY_PLUGIN_AFTER_BUILD   TRUE
)

target_sources(FionaGain
    PRIVATE
        Source/PluginProcessor.cpp
)

target_compile_definitions(FionaGain
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(FionaGain
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
