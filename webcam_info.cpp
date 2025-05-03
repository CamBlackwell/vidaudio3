#include "webcam_info.h"
#include <iostream>
#include <sstream>
#include <iomanip>


WebcamInfo::WebcamInfo() : has_previous_frame(false) {
    bool loaded = face.cascade.load(cv::samples::findFile("haarcascade/haarcascade_frontalface_alt2.xml"));
    if (!loaded) {
        std::cerr << "Error loading cascade" << std::endl;
    }
}

cv::Mat WebcamInfo::analyzeAndDisplay(const cv::Mat& frame) {
    std::string left_side = get_pixel_info();
    std::string middle_info = get_pixel_info();
    std::string right_info= get_pixel_info();

    std::vector<std::string> info_colums{
        "left side: " + left_side,
        "middle: " + middle_info,
        "right side: " + right_info,
    };
}

std::string WebcamInfo::get_pixel_info(const cv::Mat& frame, int x, int y) {
    std::stringstream ss;

    if (x >= 0 && x < frame.cols && y >= 0 && y < frame.rows) {
        if (frame.channels() == 3) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
            ss << "BGR(" << (int)pixel[0] << "," << (int)pixel[1] << "," << (int)pixel[2] << ")";
    } else if (frame.channels() == 1) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
            ss << "Gray(" << (int)pixel[0] << ")";
        }
    } else {
        ss << "Out of bounds";
    }
    return ss.str();
}

cv::Mat WebcamInfo::display_info_and_video(const cv::Mat& frame, const std::<std::string>& info_colums) {
    int line_height = 20;
    panel_height = line_height * info_colums.size();
    int font_face = cv::FONT_HERSHEY_SIMPLEX;
    double font_scale = 1.0;
    int thickness = 1;

    cv::Mat info_panel(panel_height, frame.cols, CV_8UC3, cv::Scalar(0, 0, 0));

    for (size_t i = 0; i < info_colums.size(); i++) {
        int y_position = (i + 1) * line_height - 10;
        //TODO: finish this function 