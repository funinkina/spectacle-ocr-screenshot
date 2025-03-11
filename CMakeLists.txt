cmake_minimum_required(VERSION 3.16)
project(Screenshot_OCR VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Use pkg-config for Tesseract and Leptonica
find_package(PkgConfig REQUIRED)
pkg_check_modules(Tesseract REQUIRED IMPORTED_TARGET tesseract)
pkg_check_modules(Leptonica REQUIRED IMPORTED_TARGET lept)

# Create executable
add_executable(spectacle-ocr-screenshot main.cpp)

# Link libraries
target_link_libraries(spectacle-ocr-screenshot PRIVATE
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Gui 
    PkgConfig::Tesseract
    PkgConfig::Leptonica
)
