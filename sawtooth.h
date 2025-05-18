#ifndef SAWTOOTH_H
#define SAWTOOTH_H

#include <rtaudio/RtAudio.h>

#include <atomic>
#include <cmath>
#include <iostream>
#include <string>

#include "scales.h"

struct UserData {
    std::atomic<int> leftindex{0};
    std::atomic<int> rightindex{0};
    std::atomic<int> leftoctave{0};
    std::atomic<int> rightoctave{0};
    double lastvalues[2] = {0.0, 0.0};
    std::atomic<int> key{0};
    static constexpr int keysize = Scales::keysize;
    static constexpr int scalesize = Scales::scalesize;

    double getnote(int index, int octave, int key) const {
        if (index < 0 || index >= scalesize) return 0.005;
        double freq = Scales::ALL_SCALES[key][index] * pow(2.0, octave);
        return freq / 44100.0;
    }
};

class Sawtooth {
   private:
    RtAudio dac;
    UserData userData;
    bool is_running = false;
    int new_left_octave = 0;
    int new_right_octave = 0;

    static int saw(void *outputbuffer, void *inputbuffer, unsigned int nbufferframes,
                   double streamtime, RtAudioStreamStatus status, void *userdata);

   public:
    Sawtooth();
    ~Sawtooth();

    bool initialise();
    void start();
    void stop();

    void set_left_note(int index, int octave);
    void set_right_note(int index, int octave);
    void set_key(int key);

    int get_left_index() const { return userData.leftindex.load(); }
    int get_right_index() const { return userData.rightindex.load(); }
    int get_left_octave() const { return userData.leftoctave.load(); }
    int get_right_octave() const { return userData.rightoctave.load(); }
    int get_key() const { return userData.key.load(); }

    void update_notes(int left, int right, int key, int octave);
    void run_control_thread();
};

#endif