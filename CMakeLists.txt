cmake_minimum_required(VERSION 3.8)
project(adversareval LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
find_package(Threads)

add_executable(hotwheels
    hotwheels/bitscan.hpp
    hotwheels/evaluate.cpp
    hotwheels/game.hpp
    hotwheels/game.cpp
    hotwheels/historytables.hpp
    hotwheels/historytables.cpp
    hotwheels/main.cpp
    hotwheels/movegen.hpp
    hotwheels/movegen.cpp
    hotwheels/ui.hpp
    hotwheels/ui.cpp
    hotwheels/zobrist.cpp
    minimax/minimax.cpp
    minimax/minimax.hpp
    ansi/ansi.hpp
    ansi/ansi.cpp
    ansi/prog.hpp
    ansi/prog.cpp
    transpositiontables/transpositiontbl.hpp
    transpositiontables/transpositiontbl.cpp
    transpositiontables/zobrist.hpp)
target_include_directories(hotwheels PRIVATE
    hotwheels minimax transpositiontables ansi)
target_link_libraries(hotwheels ${CMAKE_THREAD_LIBS_INIT})
