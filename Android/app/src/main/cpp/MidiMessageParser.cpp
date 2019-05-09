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
#include "MidiMessageParser.h"


MidiMessageParser::MidiMessageParser() {
}

void MidiMessageParser::parse(uint8_t *data, int offset, int numBytes) {
    // FIXME needs MIDI framing into individual messages
    uint8_t *message = &data[offset];
    int status = message[0];
    int command = status & 0xF0;
    int channel = status & 0x0F;
    int velocity = 0;

    switch (command) {
        case NOTE_ON:
            velocity = message[2];
            if (velocity == 0) {
                noteOff(channel, message[1], velocity);
            } else {
                noteOn(channel, message[1], velocity);
            }
            break;

        case NOTE_OFF:
            noteOff(channel, message[1], message[2]);
            break;

        case PITCH_BEND:
            pitchBend(channel, (message[2] << 7) | message[1]);
            break;

        default:
            break;
    }

}
