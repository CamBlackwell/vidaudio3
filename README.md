# Webcam Application

A simple C++ application that captures and displays webcam feed using OpenCV.

## Setup

1. Clone this repository:
git clone https://github.com/CamBlackwell/webcam_app.git
cd webcam_app

2. Install vcpkg:
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

3. Install OpenCV with vcpkg:
./vcpkg install opencv

## Building
mkdir build
cd build
cmake ..
cmake --build .

## Running
./Debug/webcam_app.exe