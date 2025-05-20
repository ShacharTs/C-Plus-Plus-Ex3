# -----------------------------------------------------------------------------
# Makefile for CoupGame
# -----------------------------------------------------------------------------

# Compiler and flags
CXX        := g++
CXXFLAGS   := -std=c++17 $(shell wx-config --cxxflags)
LDFLAGS    := $(shell wx-config --libs) -lsfml-audio

# Windows-specific libs
UNAME_S := $(shell uname -s)
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    LDFLAGS += -lwinmm -lcomdlg32 -lole32 -luuid -lgdi32
    TARGET_EXT := .exe
else
    TARGET_EXT :=
endif

# Build directory
BUILD_DIR    := build
BIN          := $(BUILD_DIR)/CoupGame$(TARGET_EXT)

# Source files
SRC := \
  gui/App.cpp gui/GameFrame.cpp gui/GamePanel.cpp gui/MenuFrame.cpp \
  gui/MenuPanel.cpp gui/RevealDialog.cpp \
  game/Game.cpp game/player/Player.cpp \
  game/player/roleSrc/Baron.cpp game/player/roleSrc/General.cpp \
  game/player/roleSrc/Governor.cpp game/player/roleSrc/Judge.cpp \
  game/player/roleSrc/Merchant.cpp game/player/roleSrc/Spy.cpp

# Object files (in build/obj)
OBJ_DIR      := $(BUILD_DIR)/obj
OBJ          := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

# Assets: copy everything under assets/ into build/assets/
ASSETS_SRC   := assets
ASSETS_DST   := $(BUILD_DIR)/assets

.PHONY: all clean

# Default target: build exe + copy assets
all: $(BIN) copy-assets
	@echo "Build complete → $(BIN)"

# Link step
$(BIN): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Compile step
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Copy entire assets directory into build/
copy-assets:
	@rm -rf $(ASSETS_DST)
	@mkdir -p $(ASSETS_DST)
	@cp -r $(ASSETS_SRC)/* $(ASSETS_DST)/
	@echo "Assets copied → $(ASSETS_DST)"

# Clean artifacts
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build artifacts"
