# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++17 $(shell wx-config --cxxflags)
LDFLAGS  = $(shell wx-config --libs) -lsfml-audio

# Detect platform for libraries
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    # No extra libs on Linux
else
    # Windows-specific libraries
    LDFLAGS += -lwinmm -lcomdlg32 -lole32 -luuid -lgdi32
endif

# Source files for main application
SRC      = \
  gui/App.cpp gui/GameFrame.cpp gui/GamePanel.cpp gui/MenuFrame.cpp \
  gui/MenuPanel.cpp gui/RevealDialog.cpp  \
  game/Game.cpp game/player/Player.cpp \
  game/player/roleSrc/Baron.cpp game/player/roleSrc/General.cpp \
  game/player/roleSrc/Governor.cpp game/player/roleSrc/Judge.cpp \
  game/player/roleSrc/Merchant.cpp game/player/roleSrc/Spy.cpp

# Object files for main application
OBJ      = $(SRC:.cpp=.o)
TARGET   = CoupGame

# Objects used for testing: exclude all GUI-related objects
GAME_OBJ = $(filter-out gui/%.o, $(OBJ))

# Test sources (using doctest)
TEST_SRC = test/test.cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)
TEST_BIN = test_runner

.PHONY: all test valgrind-game valgrind-gui clean

# Default build
all: $(TARGET)

# Build main executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Build and run tests
test: $(TEST_BIN)
	@echo "Running tests..."
	./$(TEST_BIN)

# Link test runner without GUI files
$(TEST_BIN): $(TEST_OBJ) $(GAME_OBJ)
	$(CXX) $(CXXFLAGS) $(TEST_OBJ) $(GAME_OBJ) -o $@ $(LDFLAGS)

# Compile rule for building object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Valgrind on game logic
valgrind-game: $(TEST_BIN)
	@echo "Running game logic tests under Valgrind..."
	valgrind --leak-check=full --track-origins=yes \
	         --log-file=valgrind-game.txt ./$(TEST_BIN)

# Valgrind on GUI application
valgrind-gui: $(TARGET)
	@echo "Running GUI application under Valgrind..."
	valgrind --leak-check=full --track-origins=yes \
	         --log-file=valgrind-gui.txt ./$(TARGET)

# Clean up build artifacts
clean:
	rm -f $(OBJ) $(TARGET) $(TEST_OBJ) $(TEST_BIN) valgrind-game.txt valgrind-gui.txt