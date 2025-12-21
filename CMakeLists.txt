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

# Collect all source files (without tests)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")

# Separate the application entry point so tests can reuse the core sources
set(MAIN_SOURCE "${PROJECT_SOURCE_DIR}/src/main.cpp")
list(REMOVE_ITEM SOURCES ${MAIN_SOURCE})

# Build the shared library with core logic
add_library(duel_core ${SOURCES} ${HEADERS})

# Application executable
add_executable(7WondersDuel ${MAIN_SOURCE})
target_link_libraries(7WondersDuel PRIVATE duel_core)

# Tests
file(GLOB_RECURSE TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")
if(TEST_SOURCES)
    enable_testing()
    add_executable(7WondersDuelTests ${TEST_SOURCES})
    target_link_libraries(7WondersDuelTests PRIVATE duel_core)
    add_test(NAME 7WondersDuelTests COMMAND 7WondersDuelTests)
endif()

# Set output directory
set_target_properties(7WondersDuel PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
)

if(TARGET 7WondersDuelTests)
    set_target_properties(7WondersDuelTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
    )
endif()

# Compiler warnings (optional but recommended)
if(MSVC)
    target_compile_options(duel_core PRIVATE /W4)
    if(TARGET 7WondersDuel)
        target_compile_options(7WondersDuel PRIVATE /W4)
    endif()
    if(TARGET 7WondersDuelTests)
        target_compile_options(7WondersDuelTests PRIVATE /W4)
    endif()
else()
    target_compile_options(duel_core PRIVATE -Wall -Wextra -Wpedantic)
    if(TARGET 7WondersDuel)
        target_compile_options(7WondersDuel PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    if(TARGET 7WondersDuelTests)
        target_compile_options(7WondersDuelTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Print configuration information
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
