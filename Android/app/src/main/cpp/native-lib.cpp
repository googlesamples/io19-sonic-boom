#include <jni.h>
#include <string>
#include "AudioEngine.h"

AudioEngine engine;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_io19_sonicboom_MainActivity_startEngine(JNIEnv *env, jobject instance) {

    engine.start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_io19_sonicboom_MainActivity_stopEngine(JNIEnv *env, jobject instance) {

    engine.stop();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_io19_sonicboom_MainActivity_onTouch(JNIEnv *env, jobject instance,
                                                     jboolean isDown) {

    engine.onTouch(isDown);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_io19_sonicboom_MainActivity_onSensorXChanged(JNIEnv *env, jobject instance,
                                                              jfloat value) {

    engine.onSensorXChanged(value);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_io19_sonicboom_MainActivity_startReadingMidi(JNIEnv *env, jobject instance,
                                                              jobject jMidiDevice) {

    AMidiDevice *midiDevice = nullptr;
    AMidiDevice_fromJava(env, jMidiDevice, &midiDevice);
    engine.startReadingMidi(midiDevice, 0);
}