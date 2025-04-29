#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cerr << "Could not open the camera" << std::endl;
        return -1;
    }

    std::string windowName = "webcam feed";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    cv::Mat frame;

    std::cout << "Video feed as started press esc to quit" << std::endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cout << "ERROR: No frame in video" << std::endl; 
            break;
    }

    cv::imshow(windowName, frame);

    int key = cv::waitKey(25);
    if (key == 27) {
        std::cout << "application terminated by user" << std::endl;
        break;
    }
    
    cap.release();
    cv::destroyAllWindows();

    return 0;
}