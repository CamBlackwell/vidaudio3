#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::videocapture cap(0);

    if (!cap.isOpened()){
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    std::string windowName = "webcam feed :)";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    std::cout << "Camera feed has started press esc to stop" << std::endl;

    while (true){
        cap >> frame;

        if (frame.empty()){
            std::cout << "ERROR: no frame in video" << std::endl;
            break;
        }
        cv::imshow(windowName, frame);
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