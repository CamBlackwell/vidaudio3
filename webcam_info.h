#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class WebcamInfo {
public:
    WebcamInfo();

    cv::Mat analyzeAndDisplay(const cv::Mat& frame);

private:
    std::string get_pixel_info(const cv::Mat& frame, int x, int y);
    cv::Mat display_info_and_video(const cv::Mat& frame, const std::vector<std::string>& info_colums);
    bool has_previous_frame;
    cv::CascadeClassifier face;
    int panel_height;
    int font_thickness;
};
