cmake_minimum_required(VERSION 3.24)
project(Your_Very_Own_DAW VERSION 0.1.0 LANGUAGES CXX)

# Enforce modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# Dependency Management: Automatically fetch and build SDL2
# ---------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.0 # Pin to a stable release
)

# Disable unnecessary SDL modules to speed up compilation
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

# Download and configure SDL2
FetchContent_MakeAvailable(SDL2)

# ---------------------------------------------------------
# Target Configuration
# ---------------------------------------------------------
# Define all source files to compile
add_executable(${PROJECT_NAME}
    source/main.cpp
    source/core/AudioDevice.cpp
    source/dsp/oscillators/SineOscillator.cpp
)

# Link the static SDL2 library to the YVO-DAW executable
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static)

# Include directories so compiler can find headers without relative path nightmares
target_include_directories(${PROJECT_NAME} PRIVATE source)