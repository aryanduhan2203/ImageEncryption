cmake_minimum_required(VERSION 3.15)
project(ImageEncryption VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)

# Consolidated header files
set(HEADERS
    include/image_encryption.hpp
    include/image_handler.hpp
)

# CLI executable (includes all implementations)
add_executable(imgenc 
    src/main.cpp
    ${HEADERS}
)

target_include_directories(imgenc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(imgenc PRIVATE
    OpenSSL::Crypto
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
)

# Tests (optional)
enable_testing()

find_package(GTest QUIET)
if(GTest_FOUND)
    # Simple test for header compilation
    add_executable(tests_minimal tests/test_minimal.cpp ${HEADERS})
    target_include_directories(tests_minimal PRIVATE include)
    target_link_libraries(tests_minimal PRIVATE
        OpenSSL::Crypto
        opencv_core
        opencv_imgcodecs
        GTest::gtest_main
    )
    add_test(NAME DirectHeaderCompilation COMMAND tests_minimal)
endif()

# Installation
install(TARGETS imgenc
    EXECUTABLE DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
