cmake_minimum_required(VERSION 3.4.1)

# Build our own native library
add_library (native-lib SHARED
        native-lib.cpp
        AudioEngine.cpp
        VoiceAllocator.cpp
        MidiMessageParser.cpp)

# Build the synth library
get_filename_component(SYNTH_DIR ${CMAKE_SOURCE_DIR}/../../../../../src ABSOLUTE)
add_library(synth SHARED
        ${SYNTH_DIR}/synth/UnitGenerator.cpp
        ${SYNTH_DIR}/synth/PitchToFrequency.cpp
        )

# Build the Oboe library
get_filename_component(OBOE_DIR ../../../../../../oboe ABSOLUTE)
add_subdirectory (${OBOE_DIR} ./oboe)

# Make the Oboe public headers available to our app
include_directories (${OBOE_DIR}/include ${SYNTH_DIR})

# Specify the libraries which our native library is dependent on, including Oboe
target_link_libraries (native-lib log oboe amidi synth)
