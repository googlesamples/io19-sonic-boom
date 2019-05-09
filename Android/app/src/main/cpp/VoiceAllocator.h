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

#ifndef MOBILEER_VOICE_ALLOCATOR_H
#define MOBILEER_VOICE_ALLOCATOR_H

#include <cstdint>
#include "MidiMessageParser.h"

/**
 * Voice allocator for a MIDI synthesizer.
 * Allocate oldest off voice or steal the oldest on voice.
 */

typedef uint8_t voice_index_t;
#define VOICE_INVALID_INDEX ((voice_index_t) -1)

class VoiceQueueNode
{
public:
    VoiceQueueNode() {}
    virtual ~VoiceQueueNode() {};
    VoiceQueueNode *next = NULL;
    VoiceQueueNode *previous = NULL;
    voice_index_t index = VOICE_INVALID_INDEX;
    uint8_t channel;
    uint8_t pitch;
};

/**
 * Simple doubly linked queue that allows removal from the middle.
 */
class VoiceQueue
{
public:
    VoiceQueue();
    virtual ~VoiceQueue() = default;

    /**
     * Add the node to the end of the queue.
     */
    void addTail(VoiceQueueNode *node);

    void remove(VoiceQueueNode *node);

    /**
     * Remove the node at the head of the queue.
     * Returns NULL if queue is empty.
     */
    VoiceQueueNode *removeHead();

private:
    VoiceQueueNode dummy; // points to head and tail or to itself
};

/**
 * Allocate synthesizer voices based on a least-recently-used algorithm.
 *
 * Note that this code is not thread safe and should only be called from one thread.
 */
class VoiceAllocator {

public:
    VoiceAllocator(int maxVoices);
    virtual ~VoiceAllocator();

    /**
     * Allocate a voice from the pool of voices.
     *
     * @param channel
     * @param pitch
     * @return
     */
    voice_index_t allocate(int channel, int pitch);

    /**
     * Find the voice that we previously allocated.
     *
     * @param channel
     * @param pitch
     * @return voice index if found or VOICE_INVALID_INDEX if not found
     */
    voice_index_t find(int channel, int pitch);

    /**
     * Release a voice so it can be reused.
     *
     * @param channel
     * @param pitch
     * @return
     */
    voice_index_t release(int channel, int pitch);

private:
    voice_index_t   mChannelPitchMap[MidiMessageParser::MAX_CHANNELS][MidiMessageParser::MAX_PITCHES];
    VoiceQueueNode *mVoiceNodes;
    VoiceQueue      mOnQueue;
    VoiceQueue      mOffQueue;
};


#endif //MOBILEER_VOICE_ALLOCATOR_H
