cmake_minimum_required(VERSION 3.5)
project(innfin_windows)

include_directories(include)
link_directories(lib)

set(SOURCE_FILES
        src/actor.c
        src/actor.h
        src/engine.c
        src/engine.h
        src/gui.c
        src/gui.h
        src/main.c
        src/map.c
        src/map.h
        src/util.c
        src/util.h)

add_executable(innfin_windows ${SOURCE_FILES})
target_link_libraries(innfin_windows libtcod-mingw)
