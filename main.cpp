#include <iostream>
#include <opencv2/opencv.hpp>
#include "webcam_info.h"

int main(){
    cv::VideoCapture cap(0);

    if (!cap.isOpened()){
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    std::string windowName = "webcam feed :)";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);

    cv::Mat frame, display_frame;
    std::cout << "Camera feed has started press esc to stop" << std::endl;

    WebcamInfo webcam_info;

    while (true){
        cap >> frame;

        if (frame.empty()){
            std::cout << "ERROR: no frame in video" << std::endl;
            break;
        }

        
        display_frame = webcam_info.analyzeAndDisplay(frame);
        cv::imshow(windowName, display_frame);
        cv::resizeWindow(windowName, display_frame.cols, display_frame.rows);


        int key = cv::waitKey(25);
        if (key == 27){
            std::cout << "application terminated by user by pressing esc" << std::endl;
            break;
        }
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}