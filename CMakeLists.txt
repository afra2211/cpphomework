cmake_minimum_required(VERSION 3.10)
project(7WondersDuel VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE HEADERS 
    "${PROJECT_SOURCE_DIR}/include/*.h"
)

# Create executable
add_executable(7WondersDuel ${SOURCES} ${HEADERS})

# Set output directory
set_target_properties(7WondersDuel PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
)

# Compiler warnings (optional but recommended)
if(MSVC)
    target_compile_options(7WondersDuel PRIVATE /W4)
else()
    target_compile_options(7WondersDuel PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Print configuration information
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
