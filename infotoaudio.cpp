#include "infotoaudio.h"
#include <iostream>

Infotoaudio::Infotoaudio() : note_duration_ms(100), is_enabled(true) {

}      

Infotoaudio::~Infotoaudio() {
}

void Infotoaudio::read_and_play(cv::Mat &frame, int x, int y, int threshold) {
    if (!is_enabled || x < 0 || y < 0 || x >= frame.cols || y >= frame.rows) {
        printf("infotoaudio::play_if_bright() is disabled or out of bounds\n");
        return;
    }

    int brightness = calculate_brightness(frame, x, y);
    if (brightness > threshold) {
        int frequency = 300 * (brightness * 5);
        Beep(frequency, note_duration_ms);
        printf("infotoaudio::play_if_bright() playing beep at %d Hz\n", frequency);
    }
}

int Infotoaudio::calculate_brightness(cv::Mat &frame, int x, int y) {
    int brightness = 0;
    if (frame.channels() == 3) {
    cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
    brightness = (pixel[0] + pixel[1] + pixel[2]) / 3;
    } else if (frame.channels() == 1) {
        brightness = frame.at<uchar>(y, x);
    }
    return brightness;
}


void Infotoaudio::set_note_duration_ms(int note_duration_ms) {
    this->note_duration_ms = note_duration_ms;
}

void Infotoaudio::is_enabled(bool is_enabled) {
    this->is_enabled = is_enabled;
}


