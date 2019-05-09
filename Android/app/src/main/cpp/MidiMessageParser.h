/*
 * Copyright 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MOBILEER_MIDI_MESSAGE_PARSER_H
#define MOBILEER_MIDI_MESSAGE_PARSER_H


#include <stdint.h>

class MidiMessageParser {
public:
    MidiMessageParser();
    virtual ~MidiMessageParser() = default;

    enum {
        MAX_CHANNELS = 16,
        MAX_PITCHES = 128,
        // Basic commands.
        NOTE_OFF = 0x80,
        NOTE_ON = 0x90,
        POLYPHONIC_AFTERTOUCH = 0xA0,
        CONTROL_CHANGE = 0xB0,
        PROGRAM_CHANGE = 0xC0,
        CHANNEL_AFTERTOUCH = 0xD0,
        CHANNEL_PRESSURE = CHANNEL_AFTERTOUCH,
        PITCH_BEND = 0xE0,
        SYSTEM_COMMON = 0xF0,
    };

    void parse(uint8_t *data, int offset, int numBytes);

    /**
     * Turn on a note.
     * @param channel 0-15
     * @param pitch note number, 0-127
     * @param velocity 0-127
     */
    virtual void noteOn(int channel, int pitch, int velocity) = 0;

    virtual void noteOff(int channel, int pitch, int velocity) = 0;

    /**
     * Bend a channel up or down.
     * @param channel 0-15
     * @param bend 14-bit pitch bend
     */
    virtual void pitchBend(int channel, int bend14) = 0;
};


#endif //MOBILEER_MIDI_MESSAGE_PARSER_H
