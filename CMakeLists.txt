cmake_minimum_required(VERSION 3.10)
project(DemoGame LANGUAGES CXX)

# Generate compile_commands.json for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(
        game
        game/player/roleHeader
)

# Define source files
set(SOURCES
        game/Game.cpp
        main.cpp
        game/player/Player.cpp
        game/player/roleSrc/Baron.cpp
        game/player/roleSrc/General.cpp
        game/player/roleSrc/Governor.cpp
        game/player/roleSrc/Merchant.cpp
        game/player/roleSrc/Spy.cpp
        game/player/roleSrc/Judge.cpp
        game/GameExceptions.hpp
        #tests/Test.cpp
)

# Optional: Add test file if needed
# set(TESTS tests/Test.cpp)

# Define the executable
add_executable(demogame
        ${SOURCES}
        # ${TESTS} # Uncomment if you want to build tests too
)
