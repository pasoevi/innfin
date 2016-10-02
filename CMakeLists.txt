cmake_minimum_required(VERSION 3.5)
project(innfin)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

include_directories("include" "c:\\sdl\\msvc\\include\\")
link_directories(lib)

set(SOURCE_FILES
        src/actor.c
        src/actor.h
        src/stairs.c
        src/stairs.h
        src/engine.c
        src/engine.h
        src/gui.c
        src/gui.h
        src/main.c
        src/map.c
        src/map.h
        src/util.c
        src/util.h
        src/monsters.c
        src/items.c
        src/items.h
        src/monsters.h
        src/player.c
        src/player.h
        src/quest.h
        src/quest.c
        src/parser.h
        src/parser.c)

file(COPY data DESTINATION .)
add_executable(innfin ${SOURCE_FILES})
target_link_libraries(innfin libtcod-mingw-debug)
