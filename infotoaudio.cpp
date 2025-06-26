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
      total_data_ref(150),
      current_key(5),
      sawtooth(sawtooth_instance) {}

Infotoaudio::~Infotoaudio() {}

int Infotoaudio::detemine_key(total_brigtness, total_colour){
    int total_data = (total_brigtness + total_colour) / 2;
    if (total_data > total_data_ref + 70) {
        total_data_ref = total_data;
        current_key++;
        return 1;
    }
    if (total_data < total_data_ref - 70) {
        total_data_ref = total_data;
        current_key--;
        return -1;
    }
    return 0;
}

int Infotoaudio::determine_octave(int octave, bool left_or_right_channel) {
    if (left_or_right_channel) {  // left channel
        if (octave > left_octave_ref + 60) {
            left_octave_ref = octave;
            left_octave_index++;
            return 1;
        }
        if (octave < left_octave_ref - 60) {
            left_octave_ref = octave;
            left_octave_index--;
            return -1;
        }
    } else {  // right channel
        if (octave > right_octave_ref + 60) {
            right_octave_ref = octave;
            right_octave_index++;
            return 1;
        }
        if (octave < right_octave_ref - 60) {
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
    int l_colour = calculate_colour(frame, x - 50, y);
    int r_colour = calculate_colour(frame, x + 50, y);
    int total_colour = (l_colour + r_colour) / 2;

    int l_brightness= calculate_brightness(frame, x - 50, y);  
    int r_brightness = calculate_brightness(frame, x + 50, y);  
    int total_brigtness = (l_brightness + r_brightness) / 2;
    sawtooth->update_notes(determine_note(l_colour, true), determine_note(r_colour, false), determine_key(total_brigtness, total_colour), determine_octave(l_brightness, true), determine_octave(r_brightness, false));
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
    
    //std::cout << "brightness: " << brightness << std::endl;
    return brightness;
}

int Infotoaudio::calculate_colour(cv::Mat &frame, int x, int y) {
    if (x < 0 || x >= frame.cols || y < 0 || y >= frame.rows) {
        return 128;
    }
    
    int b = 0, g = 0, r = 0;
    
    if (frame.channels() == 3) {
        cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
        b = pixel[0];
        g = pixel[1];
        r = pixel[2];
    } else if (frame.channels() == 1) {
        b = g = r = frame.at<uchar>(y, x);
    }

    int max_val = std::max(r, std::max(g, b));
    int min_val = std::min(r, std::min(g, b));
    int diff = max_val - min_val;
    int sum = r + g + b;
    
    if (sum < 80) return 0;      //Black
    if (sum > 700) return 200;    //White
    if (diff < 30) return 180;    //Gray
    
    float saturation = (float)diff / max_val;
    
    /*if (saturation < 0.2) {
        if (sum < 300) return 50;      //Dark gray
        else return 200;               //Light gray
    }*/
    
    if (r > g && r > b) {
        if (g > b * 1.5) {
            if (g > r * 0.8) return 60;    //Orange (high red, high green, low blue)
        }
        else if (b > g * 1.2) return 40;  //Pink/Magenta (high red, low green, high blue) 
        else return 20;                    //Pure red (high red, low everything else)
    }
    
    if (g > r && g > b) {
        if (r > b * 1.5) return 80;     //Yellow-green (high green, high red, low blue)
        //if (b > r * 1.5) return 100;    //Cyan-green (high green, high blue, low red)
        return 100;                      //Pure green (high green, low everything else)
    }
    
    if (b > r && b > g) {
        if (r > g * 1.5) return 160;    //Purple (high blue, high red, low green)
        if (g > r * 1.5) return 120;    //Cyan (high blue, high green, low red)
        return 140;                     //Pure blue (high blue, low everything else)
    }
    
    // Fallback
    return 100;
}

void Infotoaudio::set_note_duration_ms(int note_duration_ms) {
    this->note_duration_ms = note_duration_ms;
}
