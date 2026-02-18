cmake_minimum_required(VERSION 3.21)
<<<<<<< HEAD
project(untitled7)

set(CMAKE_CXX_STANDARD 23)

add_executable(untitled7 main.cpp PenSettings.h PenSize.h PenDrawing.h execution_menu.h execution_pause.h execution_pause.cpp execution_menu_extended.h execution_menu_extended.cpp)
=======
project(project)

set(CMAKE_CXX_STANDARD 23)

add_executable(project main.cpp PenSettings.h)
>>>>>>> b095a9fa5a4b64bbcdfac393e50073f302f95b3c

target_link_libraries(${PROJECT_NAME} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)