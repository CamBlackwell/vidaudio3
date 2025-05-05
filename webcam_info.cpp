#include "webcam_info.h"
#include <iostream>
#include <sstream>
#include <iomanip>



WebcamInfo::WebcamInfo() : has_previous_frame(false) {
    /*
    bool loaded = face.load(cv::samples::findFile("haarcascade/haarcascade_frontalface_alt2.xml"));
    if (!loaded) {
        std::cerr << "Error loading cascade" << std::endl;
    }
        */
}

cv::Mat WebcamInfo::analyzeAndDisplay(const cv::Mat& frame) {
    std::string left_side = get_pixel_info(frame, frame.cols / 4, frame.rows / 4);
    std::string middle_info = get_pixel_info(frame, frame.cols / 2, frame.rows / 2);
    std::string right_info= get_pixel_info(frame, 3 * frame.cols / 4, frame.rows / 4);

    std::vector<std::string> info_colums{
        "left side: " + left_side,
        "middle: " + middle_info,
        "right side: " + right_info,
    };

    return display_info_and_video(frame, info_colums);
}

std::string WebcamInfo::get_pixel_info(const cv::Mat& frame, int x, int y) {
    std::stringstream ss;

    if (x >= 0 && x < frame.cols && y >= 0 && y < frame.rows) {
        if (frame.channels() == 3) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
            ss << "BGR(" << (int)pixel[0] << "," << (int)pixel[1] << "," << (int)pixel[2] << ")";
        } else if (frame.channels() == 1) {
            uchar pixel = frame.at<uchar>(y, x);
            ss << "Gray(" << (int)pixel << ")";
        }
    } else {
        ss << "Out of bounds";
    }
    return ss.str();
}

cv::Mat WebcamInfo::display_info_and_video(const cv::Mat& frame, const std::vector<std::string>& info_colums) {
    int line_height = 20;
    panel_height = line_height * info_colums.size();
    int font_face = cv::FONT_HERSHEY_SIMPLEX;
    double font_scale = 1.0;
    int font_thickness = 1;

    cv::Mat info_panel(panel_height, frame.cols, CV_8UC3, cv::Scalar(0, 0, 0));

    for (size_t i = 0; i < info_colums.size(); i++) {
        int y_position = (i + 1) * line_height - 10;
        cv::putText(info_panel, info_colums[i], cv::Point(10, y_position), font_face, font_scale, cv::Scalar(255,255,255), font_thickness);
    }

    cv::Mat combined(panel_height + frame.rows, frame.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    frame.copyTo(combined(cv::Rect(0, 0, frame.cols, frame.rows)));
    info_panel.copyTo(combined(cv::Rect(0, frame.rows, frame.cols, panel_height)));

    return combined;
}
