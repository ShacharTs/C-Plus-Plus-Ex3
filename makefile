# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -g `wx-config --cxxflags`
LDFLAGS := `wx-config --libs`

# Source and object files
SRC := \
    gui/App.cpp \
    gui/GameFrame.cpp \
    gui/GamePanel.cpp \
    gui/MenuFrame.cpp \
    gui/MenuPanel.cpp \
    gui/RevealDialog.cpp \
    game/Game.cpp \
    game/player/Player.cpp \
    game/player/roleSrc/Baron.cpp \
    game/player/roleSrc/General.cpp \
    game/player/roleSrc/Governor.cpp \
    game/player/roleSrc/Judge.cpp \
    game/player/roleSrc/Merchant.cpp \
    game/player/roleSrc/Spy.cpp

OBJ := $(SRC:.cpp=.o)
TARGET := CoupGame

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compilation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET)

# Run target
run: all
	./$(TARGET)

.PHONY: all clean run
