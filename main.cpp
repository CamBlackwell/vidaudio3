#include <iostream>
#include <opencv2/opencv.hpp>

#include "infotoaudio.h"
#include "synth.h"
#include "webcam_info.h"

int main() {
    cv::VideoCapture cap(0);
    Synth synth;
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
    std::cout << "Controls:" << std::endl;
    std::cout << "  1 - Sine wave" << std::endl;
    std::cout << "  2 - Sawtooth wave" << std::endl;
    std::cout << "  3 - Square wave" << std::endl;
    std::cout << "  4 - Triangle wave" << std::endl;
    std::cout << "  ESC - Quit" << std::endl;

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

        int key = cv::waitKey(25) & 0xFF;

        switch (key) {
            case '1':
                synth.set_waveform(WaveformType::SINE);
                std::cout << "Switched to Sine wave" << std::endl;
                break;
            case '2':
                synth.set_waveform(WaveformType::SAW);
                std::cout << "Switched to Sawtooth wave" << std::endl;
                break;
            case '3':
                synth.set_waveform(WaveformType::SQUARE);
                std::cout << "Switched to Square wave" << std::endl;
                break;
            case '4':
                synth.set_waveform(WaveformType::TRIANGLE);
                std::cout << "Switched to Triangle wave" << std::endl;
                break;
            case 27: 
                std::cout << "Application terminated by user" << std::endl;
                goto exit_loop;
            default:
                break;
        }
    }
    exit_loop:
    synth.stop();
    cap.release();
    cv::destroyAllWindows();
    return 0;
}