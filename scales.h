#ifndef SCALES_H
#define SCALES_H

namespace Scales {
    // Circle of fifths order: C-G-D-A-E-B-F#-C#-G#-D#-A#-F
    static constexpr double C_MINOR[7]  = {261.63, 293.66, 311.13, 349.23, 392.00, 415.30, 466.16};
    static constexpr double G_MINOR[7]  = {392.00, 440.00, 466.16, 523.25, 587.33, 622.25, 698.46};
    static constexpr double D_MINOR[7]  = {293.66, 329.63, 349.23, 392.00, 440.00, 466.16, 523.25};
    static constexpr double A_MINOR[7]  = {440.00, 493.88, 523.25, 587.33, 659.25, 698.46, 783.99};
    static constexpr double E_MINOR[7]  = {329.63, 369.99, 392.00, 440.00, 493.88, 523.25, 587.33};
    static constexpr double B_MINOR[7]  = {493.88, 554.37, 587.33, 659.25, 739.99, 783.99, 880.00};
    static constexpr double FS_MINOR[7] = {369.99, 415.30, 440.00, 493.88, 554.37, 587.33, 659.25};
    static constexpr double CS_MINOR[7] = {277.18, 311.13, 329.63, 369.99, 415.30, 440.00, 493.88};
    static constexpr double GS_MINOR[7] = {415.30, 466.16, 493.88, 554.37, 622.25, 659.25, 739.99};
    static constexpr double DS_MINOR[7] = {311.13, 349.23, 369.99, 415.30, 466.16, 493.88, 554.37};
    static constexpr double AS_MINOR[7] = {466.16, 523.25, 554.37, 622.25, 698.46, 739.99, 830.61};
    static constexpr double F_MINOR[7]  = {349.23, 392.00, 415.30, 466.16, 523.25, 554.37, 622.25};

    static constexpr const double* ALL_SCALES[] = {
        C_MINOR, G_MINOR, D_MINOR, A_MINOR, E_MINOR, B_MINOR,
        FS_MINOR, CS_MINOR, GS_MINOR, DS_MINOR, AS_MINOR, F_MINOR
    };
    
    static constexpr int keySize = 12;
    static constexpr int scaleSize = 7;
}

#endif // SCALES_H