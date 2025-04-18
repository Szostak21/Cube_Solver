cmake_minimum_required(VERSION 3.10)
project(RubiksCubeSolver)

set(CMAKE_CXX_STANDARD 17)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Include your own headers if needed
include_directories(include)

# Collect all .cpp files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Build executable
add_executable(RubiksCubeSolver ${SOURCES})

# Link OpenCV libraries
target_link_libraries(RubiksCubeSolver ${OpenCV_LIBS})
