cmake_minimum_required(VERSION 3.15)

project(NoiseLab VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
endif()

# Add JUCE as subdirectory
set(JUCE_PATH "/Applications/JUCE" CACHE PATH "Path to JUCE")

# Check if we should build a headless version (no GUI)
option(HEADLESS_BUILD "Build without GUI" OFF)

# Add JUCE subdirectory
add_subdirectory(${JUCE_PATH} JUCE)

# JUCE plugin configuration
if(HEADLESS_BUILD)
    set(PLUGIN_FORMATS VST3)
else()
    set(PLUGIN_FORMATS VST3 AU Standalone)
endif()

juce_add_plugin(NoiseLab
    VERSION 1.0.0
    COMPANY_NAME "NoiseLab"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Nlab
    PLUGIN_CODE Nlb1
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "Noise Lab"
)

# Set sources
set(PLUGIN_SOURCES
    src/PluginProcessor.cpp
    src/NoiseGenerator.cpp
    src/EnvelopeGenerator.cpp
    src/LFOGenerator.cpp
    src/FilterProcessor.cpp
    src/EffectsProcessor.cpp
)

# Add editor only for non-headless builds
if(NOT HEADLESS_BUILD)
    list(APPEND PLUGIN_SOURCES src/PluginEditor.cpp)
endif()

target_sources(NoiseLab PRIVATE ${PLUGIN_SOURCES})

# JUCE modules
target_compile_definitions(NoiseLab
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# Platform-specific definitions
if(APPLE)
    target_compile_definitions(NoiseLab
        PUBLIC
        JUCE_SUPPORT_CARBON=0
    )
endif()

# For headless builds, add appropriate definitions
if(HEADLESS_BUILD)
    target_compile_definitions(NoiseLab
        PUBLIC
        JUCE_HEADLESS=1
    )
endif()

# Base libraries for all builds
set(JUCE_LIBRARIES
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
)

# Add GUI libraries only for non-headless builds
if(NOT HEADLESS_BUILD)
    list(APPEND JUCE_LIBRARIES
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    )
endif()

target_link_libraries(NoiseLab
    PRIVATE
        ${JUCE_LIBRARIES}
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Include directories
target_include_directories(NoiseLab
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${JUCE_PATH}/share/juce/modules
)

# Copy assets to build directory
add_custom_command(TARGET NoiseLab POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:NoiseLab>/assets
)