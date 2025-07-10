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

   private:
    bool has_previous_frame;
    cv::CascadeClassifier face;
    int panel_height;
    int frame_height;
    cv::Point2i button_positions[4];  // Store button positions for click detection
    cv::Rect button_rects[4];         // Store button rectangles
    const char* waveform_names[4] = {"SINE", "SAW", "SQUARE", "TRIANGLE"};

    std::string get_pixel_info(const cv::Mat& frame, int x, int y);
    cv::Mat display_info_and_video(const cv::Mat& frame, const std::vector<std::string>& info_columns,int current_waveform);
};

#endif  // WEBCAM_INFO_H