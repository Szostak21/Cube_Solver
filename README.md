# Rubik's Cube Solver

**Rubik's Cube Solver** is a C++ application that helps you solve a 3x3 Rubik's Cube using OpenCV and Kociemba's algorithm.\
It detects the cube state from your webcam and provides step-by-step instructions to solve it.

---

## Demo



---

## Features

- Real-time webcam cube scanning
- Color detection using clustering (robust to lighting changes)
- Solve cube using Kociemba's algorithm
- Display solution in **Rubik's Cube notation** or plain English instructions
- Written in C++17 with OpenCV 4.x

---

## How It Works

1. **Webcam Face Capture**\
   The program overlays a 3×3 grid on the webcam feed. Align the cube with the grid and scan each face by pressing **Space** or **Enter**. Make sure the center pieces match the grid.

2. **Color Detection via Clustering**

   - HSV values are sampled from each of the 9 stickers per face
   - Center sticker of each face is used as reference
   - Every facelet is assigned to the closest center color

3. **Cube Solving**\
   After scanning all six faces:

   - Maps each facelet to its color label: `U (Yellow), F (Orange), B (Red), L (Green), R (Blue), D (White)`
   - Uses **Kociemba's Algorithm** to compute the solving sequence

---

## Usage

1. Build the project (see [How to Build](#how-to-build))
2. Run the solver and select a display mode:
   - **Option 1:** Rubik's Cube notation (U, R, L, F, B, D)\
     Learn the notation: [Rubik's Cube Moves](https://jperm.net/3x3/moves)
   - **Option 2:** Plain English instructions
3. Scan all six faces following on-screen instructions
4. Follow the solution steps or retry scanning if necessary
5. Press **Esc** to exit

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
```

You can also build and run the project in an IDE like **CLion**.

---

## Project Structure

```
Cube_Solver/
│
├─ src/          # Source code
├─ include/      # Header files
├─ build/        # Build directory
├─ samples/      # Sample cube images (optional)
├─ CMakeLists.txt
└─ README.md
```

---

## Screenshots / GIFs

- Webcam scan overlay
- Detected cube state
- Solution output

*(Dodaj GIF lub screenshot z działania programu)*

---

## Author

**Szostak21** – [GitHub Profile](https://github.com/Szostak21)

---

## License

MIT License. See the [LICENSE](LICENSE) file for details.

