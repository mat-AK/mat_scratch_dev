cmake_minimum_required(VERSION 4.0)
project(untitled75)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled75 main.cpp
        Operators.h
        operators_test.h
        costume.h
        costume_test.h
        background.h
        background_test.h
        execution_pause_complete.h)

target_link_libraries(${PROJECT_NAME} -lconio)
target_link_libraries(${PROJECT_NAME} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(BackgroundProject main.cpp
        cmake-build-debug/MyCommon.h
        cmake-build-debug/execution_pause_complete.h)

target_link_libraries(BackgroundProject ${SDL2_LIBRARIES})




