# Cube Solver – OpenCV + Kociemba (C++17)

A production‑quality Rubik’s Cube solver for webcam input. It scans the cube live, robustly classifies sticker colors, generates a valid Kociemba string, and shows either the compact move sequence or friendly, step‑by‑step on‑screen instructions.

## 🎬 Demo

![Live demo](docs/media/demo.gif)

## ✨ Highlights

- Real‑time webcam scanning with a clean overlay grid
- Robust color clustering in HSV with balanced assignment (exactly 9 stickers per color)
- Correct face orientation and Kociemba string generation (U R F D L B)
- Python Kociemba solver integration with error suppression and validation
- Two modes: notation output or interactive, on‑screen instructions (SPACE/ENTER to advance)
- Overlay visibility toggle: camera preview stays live while UI hints hide/unhide as needed
- C++17, OpenCV 4.x, CMake; minimal dependencies and fast startup

## 🧠 How it works

1. Scan
   - A 3×3 grid overlays the live camera. Press SPACE/ENTER to capture each face.
   - The center sticker of each face is the color anchor; we sample small patches for every facelet.

2. Color clustering (HSV)
   - Distance to face centers in HSV; greedy, capacity‑aware assignment enforces 9 stickers per color.
   - Handles lighting variance better than naive RGB thresholds.

3. Orientation and string
   - Faces are rotated to match the Kociemba convention (e.g., U 90° clockwise, D 180° based on your scan).
   - Colors map to letters: Yellow→U, Blue→R, Orange→F, White→D, Green→L, Red→B.

4. Solve
   - The 54‑char string is solved via Python’s `kociemba` package; stderr is suppressed for a clean UX.
   - If invalid, the app prompts to rescan (common in poor lighting).

5. Guide
   - Mode 1: print solution (notation) immediately.
   - Mode 2: show step‑by‑step overlay on the live preview; press SPACE/ENTER to advance, ESC to exit.

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

## 🧪 Implementation notes (portfolio)

- Empty‑frame safeguards and capture order fix (cvtColor assertions avoided)
- Display mirroring for user comfort while sampling on the original frame
- Bilingual labels (EN/PL) centralized in the overlay; consistent color naming across modules
- Capacity‑aware clustering: ensures the Kociemba string has exactly 9 of each letter
- Stderr redirection for Python solver to avoid noisy tracebacks in the UI

## 🐛 Troubleshooting

- Black/empty window: check camera permissions and try a different backend (OpenCV auto‑selects; GStreamer/V4L2).
- Color misclassification: improve lighting, avoid reflections, keep stickers centered in each grid cell.
- Invalid cube string: the app will ask to rescan; ensure all faces were captured and centers are visible.
- Python not found: install Python 3 and `pip3 install kociemba`; ensure `python3` is on PATH.

## 📜 License

MIT — see [LICENSE](LICENSE).

## 🙋 About

Built by [Szostak21](https://github.com/Szostak21). If you’re looking for a developer who ships practical, camera‑driven CV apps with clean UX, let’s talk.

