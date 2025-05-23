cmake_minimum_required(VERSION 3.14)
project(CoupGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Extract wxWidgets compile flags
execute_process(
        COMMAND wx-config --cxxflags
        OUTPUT_VARIABLE WX_CXXFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND wx-config --libs
        OUTPUT_VARIABLE WX_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Split flags into a list
separate_arguments(WX_CXXFLAGS_LIST UNIX_COMMAND "${WX_CXXFLAGS}")

# Extract include directories only (just the -I flags)
set(WX_INCLUDE_DIRS "")
foreach(flag ${WX_CXXFLAGS_LIST})
    if(flag MATCHES "^-I(.+)")
        list(APPEND WX_INCLUDE_DIRS "${CMAKE_MATCH_1}")
    endif()
endforeach()

# Source files
set(SOURCES
        gui/App.cpp gui/GameFrame.cpp gui/GamePanel.cpp gui/MenuFrame.cpp
        gui/MenuPanel.cpp gui/RevealDialog.cpp
        game/Game.cpp game/player/Player.cpp
        game/player/roleSrc/Baron.cpp game/player/roleSrc/General.cpp
        game/player/roleSrc/Governor.cpp game/player/roleSrc/Judge.cpp
        game/player/roleSrc/Merchant.cpp game/player/roleSrc/Spy.cpp
        test/test.cpp
        game/player/roleHeader/role.hpp
)

# Define the executable
add_executable(CoupGame ${SOURCES})

# Apply include dirs + other compile flags
target_include_directories(CoupGame PRIVATE ${WX_INCLUDE_DIRS})
target_compile_options(CoupGame PRIVATE ${WX_CXXFLAGS_LIST})
target_link_libraries(CoupGame ${WX_LIBS})

set(SFML_DIR "C:/msys64/mingw64/lib/cmake/SFML")
find_package(SFML REQUIRED COMPONENTS graphics window system)
target_link_libraries(CoupGame sfml-graphics sfml-window sfml-system)



