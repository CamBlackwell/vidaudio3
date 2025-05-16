#include "sawtooth.h"
#include <iostream>
#include <algorithm>

Sawtooth::Sawtooth(){

}

Sawtooth::~Sawtooth(){
    stop();
}

bool Sawtooth::initialise(){
    unsigned int devicecount = dac.getDeviceCount();
    if ( devicecount < 1 ) {
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
    RtAudioErrorType errorType = dac.openStream( &parameters, NULL, RTAUDIO_FLOAT64, 
                                                  samplerate, &bufferFrames, &saw, 
                                                  (void*)&userData);
    if (errorType != RTAUDIO_NO_ERROR) {
        std::cout << "\nError opening stream: " << dac.getErrorText() << "\n" << std::endl;
        return false;
    }
    return true;
}

void Sawtooth::start(){
    if (!is_running){
        RtAudioErrorType errorType = dac.startStream();
        if (errorType != RTAUDIO_NO_ERROR) {
            std::cout << "\nError starting stream: " << dac.getErrorText() << "\n" << std::endl;
            return;
        }
        is_running = true;
    }
}

void Sawtooth::stop(){
    if (is_running && dac.isStreamRunning()){
        dac.stopStream();
        is_running = false;
    } 
    if (dac.isStreamOpen()){
        dac.closeStream();
    }
}

int Sawtooth::saw( void *outputbuffer, void *inputbuffer, unsigned int nbufferframes,
        double streamtime, RtAudioStreamStatus status, void *userdata ) {
        unsigned int i, j;
        double *buffer = (double *) outputbuffer;
        UserData *data = (UserData *) userdata;

        if ( status ) {
            std::cout << "stream underflow detected!" << std::endl;
            return 0;
        }


        for ( i=0; i<nbufferframes; i++ ) {
            double incleft = data->getnote(data->leftindex.load(), data->leftoctave.load(), data->key.load());
            double incright = data->getnote(data->rightindex.load(), data->rightoctave.load(), data->key.load());

            *buffer++ = data->lastvalues[0];
            data->lastvalues[0] += incleft;
            if (data->lastvalues[0] > 1.0) data -> lastvalues[0] -= 2.0;


            *buffer++ = data->lastvalues[1];
            data->lastvalues[1] += incright;
            if (data->lastvalues[1] > 1.0) data -> lastvalues[1] -= 2.0;
        }
    return 0;
}


void Sawtooth::set_left_note(int index, int octave) {
    userData.leftindex = std::max(0, std::min(userData.scalesize - 1, index));
    userData.leftoctave = std::max(-2, std::min(2, octave));
}

void Sawtooth::set_right_note(int index, int octave) {
    userData.rightindex = std::max(0, std::min(userData.scalesize - 1, index));
    userData.rightoctave = std::max(-2, std::min(2, octave));
}

void Sawtooth::set_key(int key) {
    userData.key = std::max(0, std::min(userData.keysize - 1, key));
}

void Sawtooth::update_notes(int left_change, int right_change, int key, int octave) {
    int new_left = userData.leftindex.load() + left_change;
    int new_right = userData.rightindex.load() + right_change;
    
    set_left_note(new_left, octave);
    set_right_note(new_right, octave);
    set_key(key);
}


