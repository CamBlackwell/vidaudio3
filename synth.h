#ifndef Synth_H
#define Synth_H

#include <rtaudio/RtAudio.h>

#include <atomic>
#include <cmath>
#include <iostream>
#include <string>

#include "scales.h"

enum class WaveformType {
    SINE = 0,
    SAW = 1,
    SQUARE = 2,
    TRIANGLE = 3
};

struct UserData {
    std::atomic<int> leftindex{0};
    std::atomic<int> rightindex{0};
    std::atomic<int> leftoctave{0};
    std::atomic<int> rightoctave{0};
    double lastvalues[2] = {0.0, 0.0};
    std::atomic<int> key{0};
    static constexpr int keysize = Scales::keysize;
    static constexpr int scalesize = Scales::scalesize;

    double phase[2] = {0.0, 0.0};  
    std::atomic<int> waveform{0};  
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double TWO_PI = 2.0 * PI;  

    double getnote(int index, int octave, int key) const {
        if (index < 0 || index >= scalesize) return 0.005;
        double freq = Scales::ALL_SCALES[key][index] * pow(2.0, octave);
        return freq / 44100.0;
    }
    double generateWaveform(double phase_increment, int channel, WaveformType type) {
    phase[channel] += phase_increment * TWO_PI;
    if (phase[channel] >= TWO_PI) {
        phase[channel] -= TWO_PI;
    }

    switch (type) {
        case WaveformType::SINE:
            return 0.3 * sin(phase[channel]);
        case WaveformType::SAW:
            return 0.3 * (2.0 * (phase[channel] / TWO_PI) - 1.0);
        case WaveformType::SQUARE:
            return 0.3 * (phase[channel] < PI ? 1.0 : -1.0);
        case WaveformType::TRIANGLE:
            if (phase[channel] < PI) {
                return 0.3 * (4.0 * (phase[channel] / TWO_PI) - 1.0);
            } else {
                return 0.3 * (3.0 - 4.0 * (phase[channel] / TWO_PI));
            }
        default:
            return 0.0;
    }
}
};

class Synth {
   private:
    RtAudio dac;
    UserData userData;
    bool is_running = false;
    int new_left_octave = 0;
    int new_right_octave = 0;

    static int waveform(void *outputbuffer, void *inputbuffer, unsigned int nbufferframes,
                   double streamtime, RtAudioStreamStatus status, void *userdata);

   public:
    Synth();
    ~Synth();

    bool initialise();
    void start();
    void stop();
    void set_waveform(WaveformType type);
    WaveformType get_waveform() const { return static_cast<WaveformType>(userData.waveform.load()); }

    void set_left_note(int index, int octave);
    void set_right_note(int index, int octave);
    void set_key(int key);

    int get_left_index() const { return userData.leftindex.load(); }
    int get_right_index() const { return userData.rightindex.load(); }
    int get_left_octave() const { return userData.leftoctave.load(); }
    int get_right_octave() const { return userData.rightoctave.load(); }
    int get_key() const { return userData.key.load(); }

    void update_notes(int left, int right, int key, int left_octave, int right_octave);
    void run_control_thread();
};

#endif