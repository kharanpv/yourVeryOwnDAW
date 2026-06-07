```markdown
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

---

## 🛠️ Build and Run Instructions

This project uses **CMake** as the configuration engine and **Ninja** + **LLVM/Clang** for a blazing-fast, 100% open-source compilation pipeline. It automatically fetches dependencies (like SDL2) so you do not need to install them manually.

### Prerequisites

You will need CMake, Ninja, and the Clang compiler installed on your system.

* **Windows:**
  * Install CMake.
  * Install the [official LLVM installer](https://github.com/llvm/llvm-project/releases) (ensure you check "Add LLVM to System PATH").
  * Install Ninja (via `winget install Ninja-build.Ninja` or downloading the binary).
* **macOS:**
  * Open Terminal and install the Apple Command Line Tools: `xcode-select --install` (This provides Clang).
  * Install CMake and Ninja via Homebrew: `brew install cmake ninja`.
* **Linux (Debian/Ubuntu):**
  * Run: `sudo apt update && sudo apt install cmake ninja-build clang lld`

### Compilation

Open your terminal in the root directory of the repository.

**1. Generate the Build Files**

Because Windows handles the Clang compiler slightly differently than Unix-based systems, use the generation command specific to your OS. This step also generates a `compile_commands.json` file for IDE autocomplete (like Zed or Neovim).

* **For Windows:**
  ```powershell
  cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  ```

* **For macOS / Linux:**
  ```bash
  cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  ```

> Optional: If you are using an editor like Zed, copy the generated `compile_commands.json` from the `build` folder to your root directory for native C++ autocomplete.

**2. Compile the Executable**

Once configured, tell Ninja to compile the project. (The very first run will take a moment to fetch and build SDL2 statically).

```bash
cmake --build build
```

### Execution

Because we use the Ninja build system, the executable is placed cleanly at the root of the `build` directory without being hidden inside `Debug` or `Release` subfolders.

Once the build reaches 100%, run your DAW:

* **Windows:**
  ```powershell
  .\build\Your_Very_Own_DAW.exe
  ```

* **macOS / Linux:**
  ```bash
  ./build/Your_Very_Own_DAW
  ```
```