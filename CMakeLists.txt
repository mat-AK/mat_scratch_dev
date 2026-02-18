cmake_minimum_required(VERSION 4.0)
project(untitled74)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled74 main.cpp
        ScrachSafety.h
        Sprite.h
        GraphicalElements.h
        GraphicalElementCode.cpp
        ControlFlow.h
        )
target_link_libraries(${PROJECT_NAME} -lconio)
target_link_libraries(${PROJECT_NAME} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)

