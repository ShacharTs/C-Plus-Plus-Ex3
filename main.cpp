#include "game/Game.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace coup;

int main() {
    vector<string> playerList = {"Bob", "Alice", "Steven", "Patrick", "Cat", "Shachar"};

    Game game (playerList);
}