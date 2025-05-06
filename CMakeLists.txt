cmake_minimum_required(VERSION 3.10)
project(DemoGame LANGUAGES CXX)

# tell CMake to dump a compile_commands.json next to your build dir
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(demogame
        #tests/Demo.cpp
        game/Game.cpp
        tests/Test.cpp
        game/player/Player.cpp
        game/player/roleHeader/General.hpp
        game/player/roleHeader/Baron.hpp
        game/player/roleHeader/Judge.hpp
        game/player/roleHeader/Governor.hpp
        game/player/roleHeader/Merchant.hpp
        game/player/roleHeader/Spy.hpp
        game/player/roleSrc/Governor.cpp
        game/player/roleSrc/Judge.cpp
        game/player/roleSrc/Merchant.cpp
        game/player/roleSrc/Spy.cpp
        game/player/roleSrc/Baron.cpp
        game/player/roleSrc/General.cpp

)
