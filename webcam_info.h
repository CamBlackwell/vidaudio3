#ifndef WEBCAM_INFO_H
#define WEBCAM_INFO_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class WebcamInfo {
   public:
    WebcamInfo();
    cv::Mat analyzeAndDisplay(const cv::Mat& frame, const std::string& scale_info, const std::string& notes_info, int current_waveform);
    int checkButtonClick(int x, int y);
    int getSensitivity() const { return sensitivity; }

   private:
    bool has_previous_frame;
    cv::CascadeClassifier face;
    int panel_height;
    int frame_height;
    cv::Point2i button_positions[4];  // Store button positions for click detection
    cv::Rect button_rects[4];         // Store button rectangles
    const char* waveform_names[4] = {"SINE", "SAW", "SQUARE", "TRIANGLE"};

    int key_sensitivity = 60;
   int octave_sensitivity = 60;
   int note_sensitivity = 30;
   static void onKeySensitivityChange(int value, void* userdata);
   static void onOctaveSensitivityChange(int value, void* userdata);
   static void onNoteSensitivityChange(int value, void* userdata);

   int getKeySensitivity() const { return key_sensitivity; }
   int getOctaveSensitivity() const { return octave_sensitivity; }
   int getNoteSensitivity() const { return note_sensitivity; }

    std::string get_pixel_info(const cv::Mat& frame, int x, int y);
    cv::Mat display_info_and_video(const cv::Mat& frame, const std::vector<std::string>& info_columns,int current_waveform);
};

#endif  // WEBCAM_INFO_H