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

#include <assert.h>
#include "VoiceAllocator.h"


VoiceQueue::VoiceQueue() {
    // dummy points to itself when the queue is empty
    dummy.next = &dummy;
    dummy.previous = &dummy;
}

void VoiceQueue::addTail(VoiceQueueNode *node) {
    VoiceQueueNode *last = dummy.previous;
    last->next = node;
    node->previous = last;
    node->next = &dummy;
    dummy.previous = node;
}

void VoiceQueue::remove(VoiceQueueNode *node) {
    node->previous->next = node->next;
    node->next->previous = node->previous;
    node->next = NULL;
    node->previous = NULL;
}

VoiceQueueNode *VoiceQueue::removeHead() {
    VoiceQueueNode *node = dummy.next;
    if (node == &dummy) {
        node = NULL; // empty list
    } else {
        remove(node);
    }
    return node;
}

VoiceAllocator::VoiceAllocator(int maxVoices) {
    assert(maxVoices < (1 << (sizeof(voice_index_t) * 8)));
    mVoiceNodes = new VoiceQueueNode[maxVoices]; // TODO separate open/close
    for (int i = 0; i < maxVoices; i++) {
        mVoiceNodes[i].index = i;
        mOffQueue.addTail(&mVoiceNodes[i]);
    }
    for (int channel = 0; channel < MidiMessageParser::MAX_CHANNELS; channel++) {
        for (int pitch = 0; pitch < MidiMessageParser::MAX_PITCHES; pitch++) {
            mChannelPitchMap[channel][pitch] = VOICE_INVALID_INDEX;
        }
    }
}

VoiceAllocator::~VoiceAllocator() {
    delete[] mVoiceNodes;
}

voice_index_t VoiceAllocator::allocate(int channel, int pitch) {
    // Look for matching voice already on.
    voice_index_t found = find(channel, pitch);
    if (found != VOICE_INVALID_INDEX) {
        // There is already a voice on for that pitch so just use it.
        return found;
    }
    // Use the oldest voice that is off.
    VoiceQueueNode *node = mOffQueue.removeHead();
    if (node == NULL) {
        // Steal the oldest voice that is on.
        node = mOnQueue.removeHead();
        mChannelPitchMap[node->channel][node->pitch] = VOICE_INVALID_INDEX;
    }
    mOnQueue.addTail(node);
    mChannelPitchMap[channel][pitch] = node->index;
    node->channel = channel;
    node->pitch = pitch;
    return node->index;
}

voice_index_t VoiceAllocator::find(int channel, int pitch) {
    return mChannelPitchMap[channel][pitch];
}

voice_index_t VoiceAllocator::release(int channel, int pitch) {
    voice_index_t found = find(channel, pitch);
    if (found != VOICE_INVALID_INDEX) {
        VoiceQueueNode *node = &mVoiceNodes[found];
        // It was on so move it to the off queue.
        mOnQueue.remove(node);
        mOffQueue.addTail(node);
        mChannelPitchMap[channel][pitch] = VOICE_INVALID_INDEX;
    }
    return found;
}
