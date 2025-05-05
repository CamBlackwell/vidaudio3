#ifndef WEBCAM_INFO_H
#define WEBCAM_INFO_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class WebcamInfo {
public:
    WebcamInfo();
    cv::Mat analyzeAndDisplay(const cv::Mat& frame);

private:
    bool has_previous_frame;
    cv::CascadeClassifier face;
    int panel_height;

    std::string get_pixel_info(const cv::Mat& frame, int x, int y);
    cv::Mat display_info_and_video(const cv::Mat& frame, const std::vector<std::string>& info_colums);
};

#endif // WEBCAM_INFO_H