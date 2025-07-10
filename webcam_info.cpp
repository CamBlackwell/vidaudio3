#include "webcam_info.h"

#include <iomanip>
#include <iostream>
#include <sstream>

WebcamInfo::WebcamInfo() : has_previous_frame(false) {
    /*
    bool loaded = face.load(cv::samples::findFile("haarcascade/haarcascade_frontalface_alt2.xml"));
    if (!loaded) {
        std::cerr << "Error loading cascade" << std::endl;
    }
        */
}

cv::Mat WebcamInfo::analyzeAndDisplay(const cv::Mat& frame, const std::string& scale_info, 
                                     const std::string& notes_info, int current_waveform) {
    cv::Mat display_frame = frame.clone();
    
    int centerX = frame.cols / 2;
    int centerY = frame.rows / 2;
    int leftX = centerX - 50;
    int rightX = centerX + 50;
    
    cv::circle(display_frame, cv::Point(leftX, centerY), 8, cv::Scalar(0, 255, 0), 2);  // Green circle for left
    cv::circle(display_frame, cv::Point(rightX, centerY), 8, cv::Scalar(0, 0, 255), 2); // Red circle for right
    cv::circle(display_frame, cv::Point(centerX, centerY), 5, cv::Scalar(255, 255, 0), 2); // Cyan circle for center
    
    cv::putText(display_frame, "L", cv::Point(leftX - 15, centerY + 5), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    cv::putText(display_frame, "R", cv::Point(rightX + 10, centerY + 5), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(display_frame, "C", cv::Point(centerX - 8, centerY - 10), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 0), 2);
    
    std::string left_side = get_pixel_info(frame, frame.cols / 4, frame.rows / 4);
    std::string middle_info = get_pixel_info(frame, frame.cols / 2, frame.rows / 2);
    std::string right_info = get_pixel_info(frame, 3 * frame.cols / 4, frame.rows / 4);

    std::vector<std::string> info_columns{
        "left:  " + left_side,
        "mid:  " + middle_info,
        "right: " + right_info,
        "", 
        scale_info,
        notes_info
    };

    return display_info_and_video(display_frame, info_columns, current_waveform);
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

cv::Mat WebcamInfo::display_info_and_video(const cv::Mat& frame,
                                           const std::vector<std::string>& info_columns,
                                           int current_waveform) {
    int line_height = 50;
    int button_height = 50;
    int button_width = 120;
    int button_spacing = 10;
    
    frame_height = frame.rows;  // Store for button click detection
    panel_height = line_height * info_columns.size() + button_height + 100;
    
    int font_face = cv::FONT_HERSHEY_SIMPLEX;
    double font_scale = 1.5;
    int font_thickness = 1;

    cv::Mat info_panel(panel_height, frame.cols, CV_8UC3, cv::Scalar(30, 30, 30));

    for (size_t i = 0; i < info_columns.size(); i++) {
        int y_position = (i + 1) * line_height - 5;
        cv::putText(info_panel, info_columns[i], cv::Point(10, y_position), font_face, font_scale,
                    cv::Scalar(255, 255, 255), font_thickness);
    }

    int button_y = line_height * info_columns.size() + 30;
    int start_x = 20;

    for (int i = 0; i < 4; i++) {
        int button_x = start_x + i * (button_width + button_spacing);
        
        button_positions[i] = cv::Point2i(button_x, button_y);
        button_rects[i] = cv::Rect(button_x, button_y, button_width, button_height);
        
        cv::Scalar button_color = (i == current_waveform) ? 
            cv::Scalar(0, 255, 0) : cv::Scalar(100, 100, 100);
        cv::Scalar text_color = (i == current_waveform) ? 
            cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);
        
        cv::rectangle(info_panel, button_rects[i], button_color, -1);
        cv::rectangle(info_panel, button_rects[i], cv::Scalar(255, 255, 255), 2);
        
        cv::Size text_size = cv::getTextSize(waveform_names[i], font_face, 0.7, font_thickness, nullptr);
        int text_x = button_x + (button_width - text_size.width) / 2;
        int text_y = button_y + (button_height + text_size.height) / 2;
        
        cv::putText(info_panel, waveform_names[i], cv::Point(text_x, text_y), 
                   font_face, 0.7, text_color, font_thickness);
    }

    cv::Mat combined(panel_height + frame.rows, frame.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    frame.copyTo(combined(cv::Rect(0, 0, frame.cols, frame.rows)));
    info_panel.copyTo(combined(cv::Rect(0, frame.rows, frame.cols, panel_height)));

    return combined;
}

int WebcamInfo::checkButtonClick(int x, int y) {
    int adjusted_y = y - frame_height;
    
    for (int i = 0; i < 4; i++) {
        if (button_rects[i].contains(cv::Point(x, adjusted_y))) {
            return i;
        }
    }
    return -1;
}
void WebcamInfo::onSensitivityChange(int value, void* userdata) {
    WebcamInfo* webcam_info = static_cast<WebcamInfo*>(userdata);
    webcam_info->sensitivity = value;
    std::cout << "Sensitivity changed to: " << value << std::endl;
}