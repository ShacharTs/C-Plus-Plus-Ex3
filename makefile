CXX = g++
CXXFLAGS = -std=c++17 -Wall -Igame -Igame/player/roleHeader

.PHONY: all clean valgrind

SRC = game/Game.cpp \
      main.cpp \
      game/player/Player.cpp \
      game/player/roleSrc/Baron.cpp \
      game/player/roleSrc/General.cpp \
      game/player/roleSrc/Governor.cpp \
      game/player/roleSrc/Judge.cpp \
      game/player/roleSrc/Merchant.cpp \
      game/player/roleSrc/Spy.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = gameApp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)
