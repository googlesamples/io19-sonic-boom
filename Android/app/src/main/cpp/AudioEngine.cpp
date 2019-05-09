/*
 * Copyright 2019 The Android Open Source Project
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

#include "AudioEngine.h"

#include "../../../../../../oboe/src/common/OboeDebug.h"


/**
 * The only MIDI messages we will process are NoteOn and NoteOff on channel 0
 * see MIDI spec for more
 */
constexpr uint8_t kStatusMask = 0xF0;
constexpr uint8_t kMidiNoteOn = 0x90;
constexpr uint8_t kMidiNoteOff = 0x80;
constexpr int kMidiBufferSize = 128;

void AudioEngine::start() {

    // Create an audio stream
    AudioStreamBuilder b;
    b.setPerformanceMode(PerformanceMode::LowLatency);
    b.setSharingMode(SharingMode::Exclusive);
    b.setChannelCount(1);

    b.setFormat(AudioFormat::Float);
    b.setCallback(this);

    // Open the stream
    Result result = b.openStream(&mStream);
    if (result != Result::OK){
        LOGE("Could not open stream. Error: %s", convertToText(result));
        return;
    }

    // TODO Add synth

    result = mStream->requestStart();
    if (result != Result::OK){
        LOGE("Could not start stream. Error: %s", convertToText(result));
        return;
    }
}

DataCallbackResult
AudioEngine::onAudioReady(  AudioStream *oboeStream,
                            void *audioData,
                            int32_t numFrames) {

    // Process MIDI data
    processMidi();
    memset(audioData, 0, (size_t)numFrames*sizeof(float));
    return DataCallbackResult::Continue;
}

void AudioEngine::stop() {

    if (mStream != nullptr) mStream->close();
}

void AudioEngine::startReadingMidi(AMidiDevice *midiDevice,
        int32_t portNumber) {

    media_status_t status = AMidiOutputPort_open(midiDevice, portNumber, &mMidiOutputPort);
    if (status == AMEDIA_OK){
        LOGD("Opened MIDI output port");
    }
}

void AudioEngine::stopReadingMidi() {

    if (mMidiOutputPort != nullptr){
        AMidiOutputPort_close(mMidiOutputPort);
    }
}

void AudioEngine::processMidi() {

    int32_t opCode = 0;
    uint8_t message[64];
    size_t messageSize = 0;
    int64_t timestamp = 0;

    // TODO: Receive MIDI from attached device
}

void AudioEngine::onSensorXChanged(float d) {

    // Input range is -1 to 1 for full 360 degree rotation
    // flat on desk = 0, pointing to ceiling = 0.5
    // Output range is 0 to 127
    if (d < 0) d = 0.01;
    if (d > 0.5) d = 0.5;
    d = d * 2 * 127;

    // TODO set filter cutoff
}

void AudioEngine::onTouch(bool isDown) {

    // TODO handle touch events
}