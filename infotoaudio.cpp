#include "infotoaudio.h"

#include <iostream>

Infotoaudio::Infotoaudio(Sawtooth *sawtooth_instance)
    : note_duration_ms(100),
      left_note_index(3),
      right_note_index(3),
      left_brightness_ref(128),
      right_brightness_ref(128),
      left_octave_ref(150),
      right_octave_ref(150),
      left_octave_index(3),
      right_octave_index(3),
      sawtooth(sawtooth_instance) {}

Infotoaudio::~Infotoaudio() {}

int Infotoaudio::determine_octave(int octave, bool left_or_right_channel) {
    if (left_or_right_channel) {  // left channel
        if (octave > left_octave_ref + 10) {
            left_octave_ref = octave;
            left_octave_index++;
            return 1;
        }
        if (octave < left_octave_ref - 10) {
            left_octave_ref = octave;
            left_octave_index--;
            return -1;
        }
    } else {  // right channel
        if (octave > right_octave_ref + 10) {
            right_octave_ref = octave;
            right_octave_index++;
            return 1;
        }
        if (octave < right_octave_ref - 10) {
            right_octave_ref = octave;
            right_octave_index--;
            return -1;
        }
    }
    return 0;
}


int Infotoaudio::determine_note(int brightness, bool left_or_right_channel) {
    if (left_or_right_channel) {  // left channel
        if (brightness > left_brightness_ref + 30) {
            left_brightness_ref = brightness;
            left_note_index++;
            return 1;
        }
        if (brightness < left_brightness_ref - 30) {
            left_brightness_ref = brightness;
            left_note_index--;
            return -1;
        }
    } else {  // right channel
        if (brightness > right_brightness_ref + 30) {
            right_brightness_ref = brightness;
            right_note_index++;
            return 1;
        }
        if (brightness < right_brightness_ref - 30) {
            right_brightness_ref = brightness;
            right_note_index--;
            return -1;
        }
    }
    return 0;
}

void Infotoaudio::set_lr_notes(cv::Mat &frame, int x, int y) {
    int l = calculate_brightness(frame, x - 50, y);  // left
    int r = calculate_brightness(frame, x + 50, y);  // right

    int l_oct = calculate_octave(frame, x - 50, y);  // left
    int r_oct = calculate_octave(frame, x + 50, y);  // right
    sawtooth->update_notes(determine_note(l, true), determine_note(r, false), 0, determine_octave(l_oct, true), determine_octave(r_oct, false));
}

int Infotoaudio::calculate_brightness(cv::Mat &frame, int x, int y) {
    int brightness = 0;
    if (x < 0 || x >= frame.cols || y < 0 || y >= frame.rows) {
        return 128;
    }
    if (frame.channels() == 3) {
        cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
        brightness = (pixel[0] + pixel[1] + pixel[2]) / 3;
    } else if (frame.channels() == 1) {
        brightness = frame.at<uchar>(y, x);
    }
    return brightness;
}

int Infotoaudio::calculate_octave(cv::Mat &frame, int x, int y) {
    int b, g, r = 0;
    if (x < 0 || x >= frame.cols || y < 0 || y >= frame.rows) {
        return 128;
    }
    if (frame.channels() == 3) {
        cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
        b = pixel[0];
        g = pixel[1];
        r = pixel[2];
    } else if (frame.channels() == 1) {
        b = g = r = frame.at<uchar>(y, x);
    }
    return (r * 5 + g * 3 + b * 2) / 10;
}


void Infotoaudio::set_note_duration_ms(int note_duration_ms) {
    this->note_duration_ms = note_duration_ms;
}
