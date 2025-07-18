#include "synth.h"

#include <algorithm>
#include <iostream>

Synth::Synth() {}

Synth::~Synth() {
    stop();
}



bool Synth::initialise() {
    unsigned int devicecount = dac.getDeviceCount();
    if (devicecount < 1) {
        std::cout << "no audio devices found!" << std::endl;
        return false;
    }
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int samplerate = 44100;
    unsigned int bufferFrames = 256;

    // RtAudio 6.0+ uses error codes instead of exceptions
    RtAudioErrorType errorType = dac.openStream(&parameters, NULL, RTAUDIO_FLOAT64, samplerate,
                                                &bufferFrames, &waveform, (void *)&userData);
    if (errorType != RTAUDIO_NO_ERROR) {
        std::cout << "\nError opening stream: " << dac.getErrorText() << "\n" << std::endl;
        return false;
    }
    return true;
}

void Synth::start() {
    if (!is_running) {
        RtAudioErrorType errorType = dac.startStream();
        if (errorType != RTAUDIO_NO_ERROR) {
            std::cout << "\nError starting stream: " << dac.getErrorText() << "\n" << std::endl;
            return;
        }
        is_running = true;
    }
}

void Synth::stop() {
    if (is_running && dac.isStreamRunning()) {
        dac.stopStream();
        is_running = false;
    }
    if (dac.isStreamOpen()) {
        dac.closeStream();
    }
}

int Synth::waveform(void *outputbuffer, void *inputbuffer, unsigned int nbufferframes,
                  double streamtime, RtAudioStreamStatus status, void *userdata) {
    unsigned int i;
    double *buffer = (double *)outputbuffer;
    UserData *data = (UserData *)userdata;

    if (status) {
        std::cout << "stream underflow detected!" << std::endl;
        return 0;
    }

    WaveformType currentWaveform = static_cast<WaveformType>(data->waveform.load());

    for (i = 0; i < nbufferframes; i++) {
        double incleft = data->getnote(data->leftindex.load(), data->leftoctave.load(), data->key.load());
        double incright = data->getnote(data->rightindex.load(), data->rightoctave.load(), data->key.load());

        *buffer++ = data->generateWaveform(incleft, 0, currentWaveform);  
        *buffer++ = data->generateWaveform(incright, 1, currentWaveform);
    }
    return 0;
}

void Synth::set_waveform(WaveformType type) {
    userData.waveform = static_cast<int>(type);
    std::cout << "Waveform changed to: " << static_cast<int>(type) << std::endl;
}

void Synth::set_left_note(int index, int octave) {
    userData.leftindex = std::max(0, std::min(userData.scalesize - 1, index));
    userData.leftoctave = std::max(-2, std::min(2, octave));
}

void Synth::set_right_note(int index, int octave) {
    userData.rightindex = std::max(0, std::min(userData.scalesize - 1, index));
    userData.rightoctave = std::max(-2, std::min(2, octave));
}

void Synth::set_key(int key) {
    userData.key = std::max(0, std::min(userData.keysize - 1, key));
}

void Synth::update_notes(int left_change, int right_change, int key, int left_octave, int right_octave) {
    int new_left = userData.leftindex.load() + left_change;
    int new_right = userData.rightindex.load() + right_change;
    int new_left_octave = userData.leftoctave.load() + left_octave;
    int new_right_octave = userData.rightoctave.load() + right_octave;
    

    set_left_note(new_left, new_left_octave);
    set_right_note(new_right, new_right_octave);
    set_key(key);
}

std::string Synth::getCurrentScaleName() const {
    const char* scale_names[] = {"C_MINOR", "G_MINOR", "D_MINOR", "A_MINOR", 
                                "E_MINOR", "B_MINOR", "F#_MINOR", "C#_MINOR",
                                "G#_MINOR", "D#_MINOR", "A#_MINOR", "F_MINOR"};
    int key = userData.key.load();
    return "Scale: " + std::string(scale_names[key]);
}

std::string Synth::getCurrentNotesInfo() const {
    const char* note_names[] = {"1", "2", "3", "4", "5", "6", "7"};
    int left_idx = userData.leftindex.load();
    int right_idx = userData.rightindex.load();
    int left_oct = userData.leftoctave.load();
    int right_oct = userData.rightoctave.load();
    
    return "Notes: L:" + std::string(note_names[left_idx]) + 
           "(o" + std::to_string(left_oct) + ") R:" + 
           std::string(note_names[right_idx]) + "(o" + std::to_string(right_oct) + ")";
}

