# Cube_Solver

**Cube Solver** is a C++ application using OpenCV library to help you solve your Rubik's Cube.
---

## How to use

### 1. Follow the instructions in "How to build" section.
This program uses Cmake so you need to build it manually or use an IDE like CLion to use it.

### 2. Select a mode by choosing either 1 or 2.
Rubik's Cube Notation is a set of labels for each move possible to do on the cube.
If you want to learn it follow this chart:
https://jperm.net/3x3/moves

Or just use option 2 which displays in plain English what faces you have to turn in order to solve the cube.

### 3. Scan the cube.
Make sure the cube fits EXACTLY in the grid, the CENTER pieces match the colors displayed on the screen and you are using the program in quite good light. 
Press space or enter to scan the face when ready. Make sure to match the new orientation of center pieces for each face.
This is crucial to get a successful scan.

### 4. Follow the solution or try again.
If you scanned the cube correctly the program will output the solution to your scramble. 
If for some case the scan didn't match any possible Rubik's Cube scramble (Because of bad light or wrong face orientation when scanning)
the camera will not close and you can try to scan the cube again right away. You can close the program by pressing esc key.

## How it works

### 1. Webcam Face Capture
The program displays a live webcam feed with a 3×3 grid overlay in the center of the screen. Each grid square represents a Rubik's Cube sticker. The user aligns the cube with the grid then scans each face by clicking space or enter key. 
Following the instructions on the screen is crucial to get a successful scan! (The orientation of each face must be correct)

### 2. Color Detection via Clustering
Instead of relying on fixed HSV thresholds (which are sensitive to lighting), the program uses a clustering-based approach:
- HSV values are sampled from each of the 9 stickers per face.
- The center sticker of each face is used as a color reference. (because the center pieces never move)
- Every facelet is assigned to the closest center color using HSV distance.

### 3. Solving Using Kociemba's Algorithm
After scanning all six faces, the program:
- Maps each facelet to its color label: **U (Yellow), F (Orange), B (Red), L (Green), R (Blue), D (White)**
- Feeds the string into the most popular and one of the fastest cube-solving algorithms - Kociemba's Algorithm and displays the solution.
---

## Requirements

- C++17
- OpenCV 4.x
- CMake

---

## How to Build

```bash
git clone https://github.com/Szostak21/Cube_Solver.git
cd Cube_Solver
mkdir build
cd build
cmake ..
make
./Cube_Solver
