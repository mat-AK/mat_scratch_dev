cmake_minimum_required(VERSION 4.0)
project(SP_Variables)

set(CMAKE_CXX_STANDARD 26)

add_executable(SP_Variables main3.cpp
        Variables.h
        Variables.cpp)
target_link_libraries(${PROJECT_NAME} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)
