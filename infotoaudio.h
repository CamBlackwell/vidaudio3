#ifndef INFOTOAUDIO_H
#define INFOTOAUDIO_H

#include <opencv2/opencv.hpp>
#include <Windows.h>

class Infotoaudio {
public:
    Infotoaudio(Sawtooth* sawtooth_instance);
    ~Infotoaudio();
    void set_note_duration_ms(int note_duration_ms);
    void set_lr_notes(cv::Mat frame, int x, int y);
private:
    int calculate_brightness(cv::Mat &frame, int x, int y);
    int determine_note(int brightness, bool left_or_right_channel);  
    int note_duration_ms;
    bool is_enabled;
    int left_note;
    int right_note;
    Sawtooth* sawtooth;
};


#endif