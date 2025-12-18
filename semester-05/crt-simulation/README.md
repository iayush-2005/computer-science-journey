# Cathode Ray Tube Simulation

An educational visualization of a Cathode Ray Tube (CRT) with two implementations:

1. **2D Schematic Animation**
2. **Interactive 3D View**

Designed for learning computer graphics, physics visualization, and computer organization concepts.

Implemented in C++ using OpenGL/FreeGLUT and built via CMake.

---

## 🚀 Features

### 🟢 2D Simulation (`CRT_2D.cpp`)

* **Schematic View:** Cross-section of a CRT with clearly labeled internal components.
* **Physics Animation:** Electron beam visualization with particle effects and realistic wobble.
* **Oscilloscope Output:** Inset viewport plotting the real-time beam trace.
* **Phased Sequence:** Visual pipeline:
  *Filament Heating → Anode Acceleration → Plate Deflection → Screen Impact*

### 🔵 3D Simulation (`CRT_3D.cpp`)

* **Interactive Camera:** Orbit and zoom controls for spatial understanding.
* **Component Visualization:** Leader-line labels for Heater, Cathode, Anodes, and X/Y Deflection Plates.
* **Volumetric Beam:** Semi-transparent beam with a dynamic deflection envelope.
* **Data HUD:** Records and plots beam impact points on the screen in real time.

---

## 🛠️ Prerequisites

### Linux (Debian / Ubuntu / Mint)

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake freeglut3-dev
```

### Other Platforms

* C++17 compliant compiler
* CMake (>= 3.16)
* OpenGL and GLUT / FreeGLUT development libraries

---

## ⚙️ Build & Run

This project uses **CMake**. The build directory can be named anything (e.g., `build`, `cmake-build-debug`).

### 1. Generate Build Files

```bash
mkdir build && cd build
cmake ..
```

### 2. Compile

```bash
cmake --build .
```

### 3. Run

**Run the 2D Simulation:**

```bash
./CRT_2D
```

**Run the 3D Simulation:**

```bash
./CRT_3D
```

## 🧩 Troubleshooting

### Wayland / HiDPI systems

If you encounter OpenGL driver or rendering issues, try forcing software rendering:

```bash
LIBGL_ALWAYS_SOFTWARE=1 ./CRT_2D
```

### WSL / Remote systems

Ensure X11 forwarding is enabled, or use an X server (for example, **VcXsrv** on Windows), so GLUT can create a display window.

---

## 📂 Repository Layout

* `CRT_2D.cpp` — Source code for the 2D schematic simulation
* `CRT_3D.cpp` — Source code for the 3D interactive simulation
* `CMakeLists.txt` — CMake configuration defining both targets
* `.gitignore` — Excludes build artifacts and IDE-specific files

---

## 📜 License

MIT License © 2025
This project is free to use, modify, and distribute for educational and personal purposes.