# Webcam Application

A simple C++ application for capturing and displaying webcam feed using OpenCV.

## Prerequisites

This project requires OpenCV which can be installed using vcpkg.

You have two options for using vcpkg:

### Option 1: Use system-wide vcpkg

1. Install vcpkg:
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

2. Install OpenCV:
.\vcpkg.exe install opencv

3. Set the VCPKG_ROOT environment variable:
setx VCPKG_ROOT "C:\path\to\your\vcpkg"

### Option 2: Use the included vcpkg submodule

1. Clone this repository with submodules:
git clone --recursive https://github.com/CamBlackwell/vidaudio3.git

Or if you've already cloned without --recursive:
git submodule update --init

2. Bootstrap vcpkg:
cd vcpkg
.\bootstrap-vcpkg.bat

3. Install OpenCV:
.\vcpkg.exe install opencv

## Building the Application
mkdir build
cd build
cmake ..
cmake --build .

## Running the Application
.\Debug\vidaudio3.exe