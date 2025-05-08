#ifndef INFOTOAUDIO_H
#define INFOTOAUDIO_H

#include <opencv2/opencv.hpp>
#include <Windows.h>

class Infotoaudio {
public:
    Infotoaudio();
    ~Infotoaudio();
    void read_and_play(cv::Mat &frame, int x, int y, int threshold);
    void set_note_duration_ms(int note_duration_ms);
    void set_is_enabled(bool is_enabled);
private:
    int calculate_brightness(cv::Mat &frame, int x, int y);
    int note_duration_ms;
    bool is_enabled;
};


#endif