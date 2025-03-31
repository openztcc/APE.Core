cmake_minimum_required(VERSION 3.14)
project(ApeCoreLib)

add_library(ApeCore STATIC
    ApeCore.cpp
    ApePng.cpp
)

target_include_directories(ApeCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ApeCore PUBLIC cxx_std_17)
