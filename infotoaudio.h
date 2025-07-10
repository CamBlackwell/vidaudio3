#ifndef INFOTOAUDIO_H
#define INFOTOAUDIO_H

#include <opencv2/opencv.hpp>

#include "synth.h"

class Infotoaudio {
   public:
    Infotoaudio(Synth* sawtooth_instance);
    ~Infotoaudio();
    void set_note_duration_ms(int note_duration_ms);
    void set_lr_notes(cv::Mat& frame, int x, int y);

   private:
    int calculate_brightness(cv::Mat& frame, int x, int y);
    int determine_key(int total_brigtness, int total_colour);
    int determine_note(int brightness, bool left_or_right_channel);
    int calculate_colour(cv::Mat& frame, int x, int y);
    int determine_octave(int octave, bool left_or_right_channel);
    int note_duration_ms;
    bool is_enabled;
    int left_note_index;       // Track current note index (0-6)
    int right_note_index;      // Track current note index (0-6)
    int left_brightness_ref;   // Track brightness for left channel
    int right_brightness_ref;  // Track brightness for right channel
    int left_octave_ref;       // Track brightness for left channel
    int right_octave_ref;      // Track brightness for right channel
    int left_octave_index;     // Track current note index (0-6)
    int right_octave_index;    // Track current note index (0-6)
    int total_data_ref;
    int current_key;
    Synth* sawtooth;
};

#endif