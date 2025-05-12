# Compiler
CXX = g++

# Compiler flags (includes wxWidgets flags via wx-config)
WX_CXXFLAGS := $(shell wx-config --cxxflags)
WX_LDFLAGS  := $(shell wx-config --libs)

CXXFLAGS = -std=c++17 -Wall -Igame -Igame/player/roleHeader $(WX_CXXFLAGS)

# List of all source files
SRC = \
    game/Game.cpp \
    game/player/Player.cpp \
    game/player/roleSrc/Baron.cpp \
    game/player/roleSrc/General.cpp \
    game/player/roleSrc/Governor.cpp \
    game/player/roleSrc/Judge.cpp \
    game/player/roleSrc/Merchant.cpp \
    game/player/roleSrc/Spy.cpp \
    App.cpp \
    GameFrame.cpp \
    #GameGui.cpp \
    GamePanel.cpp \
    MenuFrame.cpp \
    MenuPanel.cpp \
    RevealDialog.cpp \
    #main.cpp

# Convert .cpp list to .o list
OBJ = $(SRC:.cpp=.o)

# Target executable name
TARGET = gameApp

# Default build rule
all: $(TARGET)

# Link object files into final binary
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(WX_LDFLAGS)

# Rule to compile each .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up all build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

# Run under Valgrind to check for memory leaks
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)
