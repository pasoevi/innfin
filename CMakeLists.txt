# cmake_minimum_required(VERSION 3.5)
cmake_minimum_required(VERSION 3.0.2)
project(innfin)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

if (WIN32)
    include_directories("include" "c:\\sdl\\msvc\\include\\")
elseif()
    include_directories("include")
endif()

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

if (WIN32)
    target_link_libraries(innfin libtcod-mingw-debug)
else ()
    target_link_libraries(innfin libtcod_debug.so m)
endif ()

