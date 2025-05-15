#include "sawtooth.h"
#include <iostream>
#include <algorithm>

Sawtooth::Sawtooth(){

}

Sawtooth::~Sawtooth(){
    stop();
}

Sawtooth::initialise(){
    unsigned int devicecount = dac.getdevicecount();
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

    try {
        dac.openStream( &parameters, NULL, RTAUDIO_FLOAT64, samplerate, &bufferFrames, &saw, (void*)&userData);
    } catch (RtAudioError& e) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        return false;
    }

void Sawtooth::start(){
    if (!is_running){
        try {
            dac.startStream();
            is_running = true;
        } catch (RtAudioError& e) {
            std::cout << '\n' << e.getMessage() << '\n' << std::endl;
            return;
        }
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

int saw( void *outputbuffer, void *inputbuffer, unsigned int nbufferframes,
        double streamtime, rtaudiostreamstatus status, void *userdata ) {
        unsigned int i, j;
        double *buffer = (double *) outputbuffer;
        userdata *data = (userdata *) userdata;

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

void controlthread(userdata *data) {
    char ch;
    
    std::cout << "\ncontrols:" << std::endl;
    std::cout << "'a/z' - left channel note up/down" << std::endl;
    std::cout << "'s/x' - left channel octave up/down" << std::endl;
    std::cout << "'k/m' - right channel note up/down" << std::endl;
    std::cout << "'l/,' - right channel octave up/down" << std::endl;
    std::cout << "'j/n' - key up/down" << std::endl;

    std::cout << "'q' - quit" << std::endl;
    
    
    while (true) {
        std::cout << "\rleft: " << data->leftindex.load() << ":" << data->leftoctave.load()
                  << " right: " << data->rightindex.load() << ":" << data->rightoctave.load() << " > ";
        std::cin >> ch;
        
        switch (ch) {
            case 'a': data->leftindex = (data->leftindex.load() + 1) % data->scalesize; break;
            case 'z': data->leftindex = (data->leftindex.load() - 1 + data->scalesize) % data->scalesize; break;
            case 's': data->leftoctave = std::min(2, data->leftoctave.load() + 1); break; 
            case 'x': data->leftoctave = std::max(-2, data->leftoctave.load() - 1); break;
            case 'k': data->rightindex = (data->rightindex.load() + 1) % data->scalesize; break;
            case 'm': data->rightindex = (data->rightindex.load() - 1 + data->scalesize) % data->scalesize; break;
            case 'l': data->rightoctave = std::min(2, data->rightoctave.load() + 1); break; 
            case ',': data->rightoctave = std::max(-2, data->rightoctave.load() - 1); break;
            case 'j': data->key = (data->key.load() + 1) % data->keysize; break;
            case 'n': data->key = (data->key.load() - 1 + data->keysize)  % data->keysize; break;

            case 'q': return;
        }
    }
}

int main() {
    rtaudio dac;
    unsigned int devicecount = dac.getdevicecount();
    if ( devicecount < 1 ) {
        std::cout << "no audio devices found!" << std::endl;
        exit ( 0 );
    }
    rtaudio::streamparameters parameters;
    parameters.deviceid = dac.getdefaultoutputdevice();
    parameters.nchannels = 2;
    parameters.firstchannel = 0;    
    unsigned int samplerate = 44100;
    unsigned int bufferframes = 256;


    userdata userdata;

    try {
        dac.openstream( &parameters, null, rtaudio_float64, samplerate, &bufferframes, &saw, (void*)&userdata);
    } catch (rtaudioerror& e) {
        std::cout << '\n' << e.getmessage() << '\n' << std::endl;
        exit( 0 );
    }

    try {
        dac.startstream();
    } catch (rtaudioerror& e) {
        std::cout << '\n' << e.getmessage() << '\n' << std::endl;
        goto cleanup;
    }


    controlthread(&userdata);

    if ( dac.isstreamrunning() ) 
        dac.stopstream();

    cleanup:
    if ( dac.isstreamopen() )
        dac.closestream();

    return 0;
}
