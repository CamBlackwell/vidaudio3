#include <iostream>
#include <rtaudio/RtAudio.h>
#include <atomic>
#include <string>

struct UserData {
    std::atomic<double> incLeft{0.005};
    std::atomic<double> incRight{0.005};
    double lastValues[2] = {0,0};
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
            *buffer++ = data->lastValues[0];
            data->lastValues[0] += data-> incLeft.load();
            if (data->lastValues[0] > 1.0) data -> lastValues[0] -= 2.0;


            *buffer++ = data->lastValues[1];
            data->lastValues[1] += data-> incRight.load();
            if (data->lastValues[1] > 1.0) data -> lastValues[1] -= 2.0;
        }
    return 0;
}

void controlThread(UserData *data) {
    char ch;
    const double stepSize = 0.0005;
    
    std::cout << "\nControls:" << std::endl;
    std::cout << "'l' to increase left (+), 'j' to decrease left (-)" << std::endl;
    std::cout << "'r' to increase right (+), 'k' to decrease right (-)" << std::endl;
    std::cout << "'q' to quit" << std::endl;
    
    while (true) {
        std::cout << "\rLeft: " << data->incLeft.load() << " Right: " << data->incRight.load() << " > ";
        std::cin >> ch;
        
        switch (ch) {
            case 'l': data->incLeft = data->incLeft.load() + stepSize; break;
            case 'j': data->incLeft = std::max(0.0, data->incLeft.load() - stepSize); break;
            case 'r': data->incRight = data->incRight.load() + stepSize; break;
            case 'k': data->incRight = std::max(0.0, data->incRight.load() - stepSize); break;
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
    std::cout << "\nPlaying. Use commands to adjust frequencies:" << std::endl;
    std::cout << "l [value] - adjust left channel increment" << std::endl;
    std::cout << "r [value] - adjust right channel increment" << std::endl;
    std::cout << "q - quit program" << std::endl;

    controlThread(&userData);

    if ( dac.isStreamRunning() ) 
        dac.stopStream();

    cleanup:
    if ( dac.isStreamOpen() )
        dac.closeStream();

    return 0;
}
