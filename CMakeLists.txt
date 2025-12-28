cmake_minimum_required(VERSION 3.12)

project(WAV-Decoder-Encoder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS inc/Prerequisites.h
            inc/Audio.h)

set(SOURCES src/main.cpp
            src/Audio.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)





