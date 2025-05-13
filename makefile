CXX      = g++
CXXFLAGS = -std=c++17 $(shell wx-config --cxxflags)
LDFLAGS = $(shell wx-config --libs) -lsfml-audio

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    # No Windows libs needed on Linux
else
    # Windows-specific system libraries (for MSYS2/MinGW)
    LDFLAGS += -lwinmm -lcomdlg32 -lole32 -luuid -lgdi32
endif



SRC      = \
  gui/App.cpp gui/GameFrame.cpp gui/GamePanel.cpp gui/MenuFrame.cpp \
  gui/MenuPanel.cpp gui/RevealDialog.cpp  \
  game/Game.cpp game/player/Player.cpp \
  game/player/roleSrc/Baron.cpp game/player/roleSrc/General.cpp \
  game/player/roleSrc/Governor.cpp game/player/roleSrc/Judge.cpp \
  game/player/roleSrc/Merchant.cpp game/player/roleSrc/Spy.cpp

OBJ      = $(SRC:.cpp=.o)
TARGET   = CoupGame

.PHONY: all clean valgrind

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

valgrind: all
	valgrind --leak-check=full --track-origins=yes \
	         --log-file=valgrind-report.txt ./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
