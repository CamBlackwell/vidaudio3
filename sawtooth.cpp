#include <iostream>
#include <rtaudio/RtAudio.h>

int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *userData ) {
        unsigned int i, j;
        double *buffer = (double *) outputBuffer;
        double *lastValues = (double *) userData;

        if ( status ) {
            std::cout << "Stream underflow detected!" << std::endl;
            return 0;
        }

        for ( i=0; i<nBufferFrames; i++ ) {
            for ( j=0; j<2; j++ ) {
                *buffer++ = lastValues[j];

                lastValues[j] += 0.005 * (j+1+(j*0.1));
                if ( lastValues[j] > 1.0 ) lastValues[j] -= 2.0;
            }
        }
    return 0;
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
    double data[2] = {0, 0};

    try {
        dac.openStream( &parameters, NULL, RTAUDIO_FLOAT64, samplerate, &bufferFrames, &saw, (void*)&data);
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
    char input;
    std::cout << "\nPlaying. Press <ENTER> to quit." << std::endl;
    std::cin.get();

    if ( dac.isStreamRunning() ) 
        dac.stopStream();

    cleanup:
    if ( dac.isStreamOpen() )
        dac.closeStream();

    return 0;
}
