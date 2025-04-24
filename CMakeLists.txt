cmake_minimum_required(VERSION 3.10)
project(Cube_Solver)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(Cube_Solver 
    src/main.cpp
    src/vision/webcam_capture.cpp
    src/vision/grid_overlay.cpp
    src/vision/color_clustering.cpp
)

target_link_libraries(Cube_Solver ${OpenCV_LIBS})

add_executable(TestWebcam
    src/vision/test_webcam.cpp
    src/vision/webcam_capture.cpp
)

target_link_libraries(TestWebcam ${OpenCV_LIBS})

add_executable(Calibrate
    src/vision/color_calibration.cpp
    src/vision/webcam_capture.cpp
)

target_link_libraries(Calibrate ${OpenCV_LIBS})
