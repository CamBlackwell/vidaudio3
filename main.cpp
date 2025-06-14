#include <iostream>
#include <opencv2/opencv.hpp>

#include "infotoaudio.h"
#include "sawtooth.h"
#include "webcam_info.h"

int main() {
    cv::VideoCapture cap(0);
    Sawtooth synth;
    Infotoaudio audio(&synth);

    if (!synth.initialise()) {
        std::cerr << "Failed to initialize audio" << std::endl;
        return -1;
    }
    synth.start();

    if (!cap.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    std::string windowName = "webcam feed :)";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);

    cv::Mat frame, display_frame;
    std::cout << "Camera feed has started press esc to stop" << std::endl;

    WebcamInfo webcam_info;

    while (true) {
        cap >> frame;

        int frameCount = 0;
        const int UPDATE_INTERVAL = 5;

        if (frame.empty()) {
            std::cout << "ERROR: no frame in video" << std::endl;
            break;
        }

        if (!frame.empty()) {
            display_frame = webcam_info.analyzeAndDisplay(frame);
            // std::cout << "frame size: " << frame.size() << "display frame size: " <<
            // display_frame.size() << std::endl;
            cv::imshow(windowName, display_frame);
            cv::resizeWindow(windowName, display_frame.cols, display_frame.rows);
        }

        if (frameCount % UPDATE_INTERVAL == 0) {
            int centerX = frame.cols / 2;
            int centerY = frame.rows / 2;
            audio.set_lr_notes(frame, centerX, centerY);
        }

        frameCount++;

        int key = cv::waitKey(25);
        if (key == 27) {
            std::cout << "application terminated by user by pressing esc" << std::endl;
            break;
        }
    }
    synth.stop();
    cap.release();
    cv::destroyAllWindows();
    return 0;
}