cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(FROMFILETOWAVE VERSION 0.0.1)
add_subdirectory(JUCE)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# ===================================================================================================
# PLGUINS 
# ===================================================================================================
juce_add_plugin(FromWaveToFile
    VERSION 0.1
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME SonoTron
    # IS_SYNTH TRUE/FALSE
    # NEEDS_MIDI_INPUT TRUE/FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE SNTN
    PLUGIN_CODE FWTF
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "FromWaveToFile")

juce_generate_juce_header(FromWaveToFile)

target_sources(FromWaveToFile
    PRIVATE
        ${SRC_FILES}
    )

target_include_directories(FromWaveToFile PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_compile_definitions(FromWaveToFile
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(FromWaveToFile
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Disable -Wswitch-enum warning for all compilers
# This warning requires exhaustive case handling for all enum values, even when a default case is present
# We need to modify the interface compile options of juce_recommended_warning_flags to remove -Wswitch-enum
# and then add -Wno-switch-enum
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    get_target_property(WARNING_FLAGS juce_recommended_warning_flags INTERFACE_COMPILE_OPTIONS)
    list(REMOVE_ITEM WARNING_FLAGS "-Wswitch-enum")
    list(APPEND WARNING_FLAGS "-Wno-switch-enum")
    set_target_properties(juce_recommended_warning_flags PROPERTIES INTERFACE_COMPILE_OPTIONS "${WARNING_FLAGS}")
endif()
