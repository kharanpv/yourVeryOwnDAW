# Your Very Own DAW-less DAW

Open Source DAW meant to be used DAWless on your own computer.

## The Concept

"DAW-less" music production usually means escaping the computer screen to rely entirely on hardware—grooveboxes, synthesizers, and samplers. This project flips that concept. It is a software-based digital audio workstation designed to look, feel, and operate like standalone hardware, running directly on your computer. 

Instead of endless menus, complex VST management, and the visual distractions of a traditional desktop interface, this environment provides a constrained, tactile-feeling layout. It turns your PC into a dedicated instrument rather than a workstation.

## Why This Project?

Modern DAWs offer infinite possibilities, which often leads to creative paralysis. The goal here is to build an environment that forces focus through deliberate limitations. 

This project exists to:
* **Minimize Friction:** Eliminate the setup bloat of traditional production software to get straight to making music.
* **Emulate Hardware Workflow:** Capture the muscle-memory and performance-focused immediacy of hardware gear, utilizing the processing power of a modern computer.
* **Keep it Open & Accessible:** Create a fully open-source alternative for creators who want a minimalist, distraction-free environment without investing thousands in physical hardware.


## 🛠️ Build and Run Instructions

This project uses CMake to handle cross-platform builds and automatically fetch dependencies (like SDL2). You do not need to install SDL2 manually on your system.

### Prerequisites
* **Windows:** Visual Studio Build Tools (MSVC) and CMake.
* **Linux / Raspberry Pi:** `build-essential` (GCC/G++) and `cmake`.
* **macOS:** Xcode Command Line Tools and CMake.

### Compilation
Open your terminal in the root directory of the repository and run the following commands:

1. **Generate the build system:**
   ```bash
   cmake -S . -B build

## 2. Compile the Executable

```bash
cmake --build build
```

---

# Execution

Once the build reaches 100%, run the executable using the command for your operating system.

## Windows

If you are using the standard Microsoft Visual C++ (MSVC) compiler, CMake places the executable inside a configuration folder:

```powershell
.\build\Debug\YVO-DAW.exe
```

If you generated the build using Ninja or MinGW instead, it may be located directly at:

```powershell
.\build\Your_Very_Own_DAW.exe
```

## Linux/macOS

```bash
./build/Your_Very_Own_DAW
```
