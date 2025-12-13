# Cube Solver – OpenCV + Kociemba (C++17)

**Cube_Solver** is a C++ application using OpenCV to scan a real Rubik's Cube via webcam, detect facelet colors robustly, and convert the detected configuration into a format compatible with the Kociemba two-phase solving algorithm.

## 🎬 Demo

![Live demo](docs/media/demo.gif)

## ✨ Highlights

- Real‑time webcam scanning with a clean overlay grid
- Two languages to choose: English and Polish
- Two modes: Rubik's Cube notation output or interactive, on‑screen instructions (SPACE/ENTER to advance)
- Robust color clustering in HSV with balanced assignment (exactly 9 stickers per color)
- Python Kociemba solver integration for maximum efficiency
- C++17, OpenCV 4.x, CMake; minimal dependencies and fast startup

## 🧠 How it works

1. Scanning
   - A 3×3 grid overlays the live camera. Press SPACE/ENTER to capture each face.
   - The center sticker of each face is the color anchor; we sample small patches for every facelet.

2. Color clustering (HSV)
   - Distance to face centers in HSV; greedy, capacity‑aware assignment enforces 9 stickers per color.
   - Handles lighting variance better than naive RGB thresholds.
   - Colors map to letters: Yellow→U, Blue→R, Orange→F, White→D, Green→L, Red→B.

4. Solving
   - The 54‑char string is solved via Python’s `kociemba` package; stderr is suppressed for a clean UX.
   - If impossible state of cube was scanned (because of poor lightning etc.), the app prompts to rescan.

5. Displaying the solution
   - Mode 1 — For those who know Rubik’s Cube notation: shows compact moves (U, R, L, F, B, D). Learn the notation: https://jperm.net/3x3/moves
   - Mode 2 — For those who don’t: displays clear, localized on‑screen steps you can follow, advancing with SPACE/ENTER.

## 🛠 Tech stack

- Language: C++17
- CV/GUI: OpenCV 4.x (videoio, imgproc, highgui)
- Build: CMake
- Solver: Python 3 `kociemba` (subprocess call)

## ▶️ Quick start

Prereqs (Ubuntu/Debian):

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libopencv-dev python3 python3-pip
pip3 install --user kociemba
```

Build & run:

```bash
git clone https://github.com/Szostak21/Cube_Solver.git
cd Cube_Solver
mkdir -p build && cd build
cmake ..
cmake --build .
./Cube_Solver
```

Controls:

- SPACE/ENTER: scan a face (during scanning) or next step (in instructions mode)
- ESC: exit the preview

## 📁 Project layout

```
include/           # Public headers (overlay, capture, clustering, kociemba string)
src/
  main.cpp         # App entry: capture loop, overlays, modes, solver
  vision/          # Grid overlay, webcam capture, color sampling
  solver/          # Kociemba invocation (Python bridge)
  solver/*.cpp
docs/media/demo.gif
CMakeLists.txt
```

## 📜 License

MIT — see [LICENSE](LICENSE).
