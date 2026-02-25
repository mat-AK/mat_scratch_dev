cmake_minimum_required(VERSION 4.0)
project(mat_scratch_dev)

set(CMAKE_CXX_STANDARD 26)

add_executable(mat_scratch_dev main.cpp
        ControlFlow.cpp
        ControlFlow.h
        drawing_tools.cpp
        drawing_tools.h
        event_system.cpp
        event_system.h
        execution_menu_extended.h
        execution_pause.cpp
        execution_pause.h
        GraphicalElementCode.cpp
        GraphicalElements.h
        MyEnums.h
        MyStructs.h
        PenDrawing.h
        PenSettings.h
        PenSize.h
        ScrachSafety.h
        Sprite.h
        sprite_events.h
        sprites_events.cpp
        sprites_events.h
        Variables.h
        Variables.cpp
        main3.cpp
)

target_link_libraries(${PROJECT_NAME} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)