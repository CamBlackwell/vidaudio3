#include <iostream>
#include <rtaudio/RtAudio.h>
#include <atomic>
#include <string>
#include <cmath>
#include "scales.h"

struct UserData {
    std::atomic<int> leftIndex{0};
    std::atomic<int> rightIndex{0};
    std::atomic<int> leftOctave{0};
    std::atomic<int> rightOctave{0};
    double lastValues[2] = {0.0, 0.0};
    std::atomic<int> key{0};
    static constexpr int keySize = Scales::keySize;
    static constexpr int scaleSize = Scales::scaleSize;

    
    double getNote(int index, int octave, int key) const {
        if (index < 0 || index >= scaleSize) return 0.005; 
        double freq = Scales::ALL_SCALES[key][index] * pow(2.0, octave);
        return freq / 44100.0;
    }
};

int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *userData ) {
        unsigned int i, j;
        double *buffer = (double *) outputBuffer;
        UserData *data = (UserData *) userData;

        if ( status ) {
            std::cout << "Stream underflow detected!" << std::endl;
            return 0;
        }


        for ( i=0; i<nBufferFrames; i++ ) {
            double incLeft = data->getNote(data->leftIndex.load(), data->leftOctave.load(), data->key.load());
            double incRight = data->getNote(data->rightIndex.load(), data->rightOctave.load(), data->key.load());

            *buffer++ = data->lastValues[0];
            data->lastValues[0] += incLeft;
            if (data->lastValues[0] > 1.0) data -> lastValues[0] -= 2.0;


            *buffer++ = data->lastValues[1];
            data->lastValues[1] += incRight;
            if (data->lastValues[1] > 1.0) data -> lastValues[1] -= 2.0;
        }
    return 0;
}

void controlThread(UserData *data) {
    char ch;
    
    std::cout << "\nControls:" << std::endl;
    std::cout << "'a/z' - Left channel note up/down" << std::endl;
    std::cout << "'s/x' - Left channel octave up/down" << std::endl;
    std::cout << "'k/m' - Right channel note up/down" << std::endl;
    std::cout << "'l/,' - Right channel octave up/down" << std::endl;
    std::cout << "'j/n' - Right channel octave up/down" << std::endl;
    std::cout << "'q' - quit" << std::endl;
    
    
    while (true) {
        std::cout << "\rLeft: " << data->leftIndex.load() << ":" << data->leftOctave.load()
                  << " Right: " << data->rightIndex.load() << ":" << data->rightOctave.load() << " > ";
        std::cin >> ch;
        
        switch (ch) {
            case 'a': data->leftIndex = (data->leftIndex.load() + 1) % data->scaleSize; break;
            case 'z': data->leftIndex = (data->leftIndex.load() - 1 + data->scaleSize) % data->scaleSize; break;
            case 's': data->leftOctave = std::min(2, data->leftOctave.load() + 1); break; 
            case 'x': data->leftOctave = std::max(-2, data->leftOctave.load() - 1); break;
            case 'k': data->rightIndex = (data->rightIndex.load() + 1) % data->scaleSize; break;
            case 'm': data->rightIndex = (data->rightIndex.load() - 1 + data->scaleSize) % data->scaleSize; break;
            case 'l': data->rightOctave = std::min(2, data->rightOctave.load() + 1); break; 
            case ',': data->rightOctave = std::max(-2, data->rightOctave.load() - 1); break;
            case 'j': data->key = (data->key.load() + 1) % data->keySize; break;
            case 'n': data->key = std::max(0, data->key.load() - 1  % data->keySize); break;
            case 'q': return;
        }
    }
}

int main() {
    RtAudio dac;
    unsigned int deviceCount = dac.getDeviceCount();
    if ( deviceCount < 1 ) {
        std::cout << "No audio devices found!" << std::endl;
        exit ( 0 );
    }
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;    
    unsigned int samplerate = 44100;
    unsigned int bufferFrames = 256;


    UserData userData;

    try {
        dac.openStream( &parameters, NULL, RTAUDIO_FLOAT64, samplerate, &bufferFrames, &saw, (void*)&userData);
    } catch (RtAudioError& e) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        exit( 0 );
    }

    try {
        dac.startStream();
    } catch (RtAudioError& e) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        goto cleanup;
    }


    controlThread(&userData);

    if ( dac.isStreamRunning() ) 
        dac.stopStream();

    cleanup:
    if ( dac.isStreamOpen() )
        dac.closeStream();

    return 0;
}
