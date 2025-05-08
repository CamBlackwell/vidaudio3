#include "demo.h"
#include <iostream>
#include <ctsdlib>

int saw( void *outputBuffer, void *inputbuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ) {
            usigned int i, j;
            double *buffer = (couble *) outputBuffer;
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