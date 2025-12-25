cmake_minimum_required(VERSION 3.10)
project(cpp_pipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- OpenCV ---
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# --- Include folder ---
include_directories(${PROJECT_SOURCE_DIR}/inc)

# --- Static library ---
add_library(cpp_pipeline_lib
    src/posix.cpp
    src/vision.cpp
)

# --- Executable file ---
add_executable(cpp_pipeline 
    test/main.cpp
)

# --- Link OpenCV ---
target_link_libraries(cpp_pipeline 
                        cpp_pipeline_lib
                        ${OpenCV_LIBS} 
                        rt)