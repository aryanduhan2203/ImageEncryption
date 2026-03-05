cmake_minimum_required(VERSION 3.15)
project(ImageEncryption VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)

# Header files
set(HEADERS
    include/image_encryption.hpp
    include/image_handler.hpp
)

# Main executable
add_executable(imgenc 
    src/main.cpp
    src/image_encryption.cpp
)

target_include_directories(imgenc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(imgenc PRIVATE
    OpenSSL::Crypto
    ${OpenCV_LIBS}
)

# Testing (optional)
enable_testing()

find_package(GTest QUIET)
if(GTest_FOUND)
    add_executable(tests_minimal tests/test_minimal.cpp ${HEADERS})
    target_include_directories(tests_minimal PRIVATE include)
    target_link_libraries(tests_minimal PRIVATE
        OpenSSL::Crypto
        ${OpenCV_LIBS}
        GTest::gtest_main
    )
    add_test(NAME DirectHeaderCompilation COMMAND tests_minimal)
endif()

# Installation
install(TARGETS imgenc
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)