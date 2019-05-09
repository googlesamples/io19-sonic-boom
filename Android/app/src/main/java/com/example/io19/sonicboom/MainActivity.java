package com.example.io19.sonicboom;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.midi.MidiDevice;
import android.media.midi.MidiDeviceInfo;
import android.media.midi.MidiManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

/**
 * TODO:
 *
 * - The synth needs to output in STEREO for the lowest latency, it's currently doing so in mono
 * - see log entry: AudioStreamInternal_Client: open() - openStream() returned -889, try switching from MONO to STEREO
 * - Figure out why there's a clicking sound at low cutoff frequency - see LinearRamp from Phil
 *
 * - Remove nav buttons (in case I accidentally touch them during performance)
 * - Getting warning: Disagreement between codeName='Q' and minSdkVersion='28'. Only one should be specified.
 */
public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private static final String TAG = MainActivity.class.getName();
    private MidiManager mMidiManager;

    private native void startEngine();
    private native void stopEngine();
    private native void onTouch(boolean isDown);
    private native void onSensorXChanged(float value);
    private native void startReadingMidi(MidiDevice midiDevice);


    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }

    @Override
    protected void onResume(){
        super.onResume();
        keepScreenOn();
        setupRotationSensor();

        startEngine();
        handleMidiDevices();
    }

    @Override
    protected void onPause() {
        super.onPause();

        stopEngine();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN){
            onTouch(true);
        } else if (event.getAction() == MotionEvent.ACTION_UP) {
            onTouch(false);
        }
        return true; // event consumed
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {

        float val = sensorEvent.values[0];
        onSensorXChanged(val);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {
        // Not used
    }

    private void keepScreenOn() {
        Window window = getWindow();
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    private void setupRotationSensor() {
        SensorManager sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        // More info: https://source.android.com/devices/sensors/sensor-types#game_rotation_vector
        // Note: we use the GAME_ROTATION_VECTOR rather than the ROTATION_VECTOR because we don't
        // want the rotation axis to change based on the direction the phone is pointing.
        Sensor rotationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
        sensorManager.registerListener(this, rotationSensor, SensorManager.SENSOR_DELAY_FASTEST)  ;
    }

    private void handleMidiDevices() {

        mMidiManager = (MidiManager) getSystemService(Context.MIDI_SERVICE);
        mMidiManager.registerDeviceCallback(new MidiManager.DeviceCallback(){
            @Override
            public void onDeviceAdded(MidiDeviceInfo device) {

                // open this device
                mMidiManager.openDevice(device, new MidiManager.OnDeviceOpenedListener() {
                    @Override
                    public void onDeviceOpened(MidiDevice midiDevice) {

                        Log.d(TAG, "Opened MIDI device");
                        // Start reading from this device
                        startReadingMidi(midiDevice);
                    }
                }, new Handler());
            }

            @Override
            public void onDeviceRemoved(MidiDeviceInfo device) {
                // Stop reading from this device
            }

        }, new Handler());
    }
}
