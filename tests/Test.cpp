#include "../game/Game.hpp"


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace std;
using namespace coup;
vector<string> playerList = {"Bob", "Alice", "Steven", "Patrick", "Cat", "Shachar"};

TEST_CASE("Test pre-made players have correct roles") {
    Game game(playerList); // true for game test only

    const auto& players = game.getPlayers();
    REQUIRE(players.size() == 6);

    CHECK(players[0]->getRole() == Role::Governor);  // Bob
    CHECK(players[1]->getRole() == Role::Spy);       // Alice
    CHECK(players[2]->getRole() == Role::Baron);     // Steven
    CHECK(players[3]->getRole() == Role::General);   // Patrick
    CHECK(players[4]->getRole() == Role::Judge);     // Cat
    CHECK(players[5]->getRole() == Role::Merchant);  // Shachar
}




