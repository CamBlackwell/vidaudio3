#include <iostream>
#include <opencv2/opencv.hpp>

#include "infotoaudio.h"
#include "synth.h"
#include "webcam_info.h"

WebcamInfo* global_webcam_info = nullptr;
Synth* global_synth = nullptr;

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN && global_webcam_info && global_synth) {
        int button_clicked = global_webcam_info->checkButtonClick(x, y);
        if (button_clicked >= 0) {
            global_synth->set_waveform(static_cast<WaveformType>(button_clicked));
            std::cout << "Switched to " << button_clicked << " waveform via button click" << std::endl;
        }
    }
}

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

    WebcamInfo webcam_info;

    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    

    cv::Mat frame, display_frame;
    std::cout << "Camera feed has started press esc to stop" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  1 - Sine wave" << std::endl;
    std::cout << "  2 - Sawtooth wave" << std::endl;
    std::cout << "  3 - Square wave" << std::endl;
    std::cout << "  4 - Triangle wave" << std::endl;
    std::cout << "  ESC - Quit" << std::endl;


    
    cv::createTrackbar("Key Sensitivity", windowName, &webcam_info.getKeySensitivityRef(), 200, WebcamInfo::onKeySensitivityChange, &webcam_info);
    cv::createTrackbar("Octave Sensitivity", windowName, &webcam_info.getOctaveSensitivityRef(), 200, WebcamInfo::onOctaveSensitivityChange, &webcam_info);
    cv::createTrackbar("Note Sensitivity", windowName, &webcam_info.getNoteSensitivityRef(), 100, WebcamInfo::onNoteSensitivityChange, &webcam_info);
    cv::setMouseCallback(windowName, onMouse, nullptr);

    while (true) {
        cap >> frame;

        int frameCount = 0;
        const int UPDATE_INTERVAL = 5;

        if (frame.empty()) {
            std::cout << "ERROR: no frame in video" << std::endl;
            break;
        }

        if (!frame.empty()) {
            std::string scale_info = synth.getCurrentScaleName();
            std::string notes_info = synth.getCurrentNotesInfo();
            display_frame = webcam_info.analyzeAndDisplay(frame, scale_info, notes_info, static_cast<int>(synth.get_waveform()));
            cv::imshow(windowName, display_frame);
            cv::resizeWindow(windowName, display_frame.cols, display_frame.rows);
        }

        if (frameCount % UPDATE_INTERVAL == 0) {
            audio.setKeySensitivity(webcam_info.getKeySensitivityRef());
            audio.setOctaveSensitivity(webcam_info.getOctaveSensitivityRef());
            audio.setNoteSensitivity(webcam_info.getNoteSensitivityRef());
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