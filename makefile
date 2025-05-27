# -----------------------------------------------------------------------------
# Makefile for CoupGame (with tests & Valgrind)
# -----------------------------------------------------------------------------

# Compiler and flags
CXX        := g++
CXXFLAGS   := -std=c++17 $(shell wx-config --cxxflags)
LDFLAGS    := $(shell wx-config --libs) -lsfml-audio

# Windows-specific libs
UNAME_S := $(shell uname -s)
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    LDFLAGS    += -lwinmm -lcomdlg32 -lole32 -luuid -lgdi32
    TARGET_EXT := .exe
else
    TARGET_EXT :=
endif

# Directories
BUILD_DIR   := build
OBJ_DIR     := $(BUILD_DIR)/obj
ASSETS_SRC  := assets
ASSETS_DST  := $(BUILD_DIR)/assets

# Main executable
BIN := $(BUILD_DIR)/CoupGame$(TARGET_EXT)

# Sources
SRC := \
  gui/App.cpp gui/GameFrame.cpp gui/GamePanel.cpp gui/MenuFrame.cpp \
  gui/MenuPanel.cpp \
  game/Game.cpp game/player/Player.cpp \
  game/player/roleSrc/Baron.cpp game/player/roleSrc/General.cpp \
  game/player/roleSrc/Governor.cpp game/player/roleSrc/Judge.cpp \
  game/player/roleSrc/Merchant.cpp game/player/roleSrc/Spy.cpp

# Object files
OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

# Test runner
TEST_SRC := test/test.cpp
TEST_OBJ := $(OBJ_DIR)/test/test.o
# exclude GUI objects from game logic
GAME_OBJ := $(filter-out $(OBJ_DIR)/gui/%.o,$(OBJ))
TEST_BIN := $(BUILD_DIR)/test_runner$(TARGET_EXT)

.PHONY: all test valgrind-test valgrind-gui clean

# Default: build app + assets
all: $(BIN) copy-assets
	@echo "Build complete → $(BIN)"

# Link main executable
$(BIN): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Compile step for main objects
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Copy all assets into build/
copy-assets:
	@rm -rf $(ASSETS_DST)
	@mkdir -p $(ASSETS_DST)
	@cp -r $(ASSETS_SRC)/* $(ASSETS_DST)/
	@echo "Assets copied → $(ASSETS_DST)"

# -------------------
# Testing targets
# -------------------

# Build test runner
$(TEST_BIN): $(TEST_OBJ) $(GAME_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile test object
$(TEST_OBJ): $(TEST_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
test: $(TEST_BIN)
	@echo "Running tests..."
	@./$(TEST_BIN)

# Valgrind on logic tests
valgrind-test: $(TEST_BIN)
	@echo "Running game logic tests under Valgrind..."
	valgrind --leak-check=full --track-origins=yes \
	         --log-file=$(BUILD_DIR)/valgrind-test.txt \
	         ./$(TEST_BIN)
	@echo "Valgrind report → $(BUILD_DIR)/valgrind-test.txt"

# Valgrind on GUI app
valgrind-gui: $(BIN)
	@echo "Running GUI application under Valgrind..."
	valgrind --leak-check=full \
		--track-origins=yes \
		--errors-for-leak-kinds=definite \
		--error-exitcode=1 \
		--log-file=$(BUILD_DIR)/valgrind-gui.txt \
		./$(BIN)
	@echo "Valgrind report → $(BUILD_DIR)/valgrind-gui.txt"





# Clean everything
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build artifacts"
