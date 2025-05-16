#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../game/Game.hpp"
#include "../game/player/roleHeader/Baron.hpp"
#include "../game/player/roleHeader/General.hpp"
#include "../game/player/roleHeader/Governor.hpp"
#include "../game/player/roleHeader/Judge.hpp"
#include "../game/player/roleHeader/Merchant.hpp"
#include "../game/player/roleHeader/Spy.hpp"
#include "../game/GameExceptions.hpp"

using namespace coup;
using namespace std;

static vector<string> names = {"Bob","Alice","Steven","Patrick","Cat","Shachar"};

TEST_CASE("Initial roles assigned correctly via Game constructor") {
    cout << "[TEST] Initial roles assigned correctly via Game constructor" << endl;
    Game game(names);
    auto players = game.getPlayers();
    REQUIRE(players.size() == names.size());
    CHECK(players[0]->getRole() == Role::Governor);
    CHECK(players[1]->getRole() == Role::Spy);
    CHECK(players[2]->getRole() == Role::Baron);
    CHECK(players[3]->getRole() == Role::General);
    CHECK(players[4]->getRole() == Role::Judge);
    CHECK(players[5]->getRole() == Role::Merchant);
}

TEST_CASE("Gather and Tax increase coins and consume turn") {
    cout << "[TEST] Gather and Tax increase coins and consume turn" << endl;
    Game game(names);
    auto p = game.getPlayers()[0];
    int before = p->getCoins();
    game.gather(p);
    CHECK(p->getCoins() == before + 1);
    // gather not allowed twice
    CHECK_THROWS_AS(game.gather(p), GatherError);
    // advance turn
    game.nextTurn();
    auto p1 = game.getPlayers()[1];
    before = p1->getCoins();
    game.tax(p1);
    CHECK(p1->getCoins() == before + 2);
}

TEST_CASE("Bribe grants extra turn and cost deducted") {
    cout << "[TEST] Bribe grants extra turn and cost deducted" << endl;
    Game game(names);
    auto p = game.getPlayers()[2]; // Baron
    p->addCoins(5);
    game.bribe(p);
    CHECK(p->hasExtraTurn());
    CHECK(p->getCoins() == 5 - 4);
}

TEST_CASE("Arrest steals coin and records last arrested") {
    cout << "[TEST] Arrest steals coin and records last arrested" << endl;
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    p0->addCoins(1);          // Give arresting player coin to gain
    p1->addCoins(1);          // Give target a coin to lose
    game.arrest(p0, p1);
    // After arrest: p0 gains 1, p1 loses 1
    CHECK(p0->getCoins() == 2);
    CHECK(p1->getCoins() == 0);
    // Self-arrest should throw
    CHECK_THROWS_AS(game.arrest(p0, p0), SelfError);
}

TEST_CASE("Sanction deducts cost and disables target gather/tax") {
    cout << "[TEST] Sanction deducts cost and disables target gather/tax" << endl;
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    p0->addCoins(3);
    game.sanction(p0, p1);
    CHECK(!p1->isGatherAllow());
    CHECK(!p1->isTaxAllow());
    CHECK(p0->getCoins() == 0);
}

TEST_CASE("Coup removes target and costs coins") {
    cout << "[TEST] Coup removes target and costs coins" << endl;
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    p0->addCoins(7);
    game.coup(p0, p1);
    auto players = game.getPlayers();
    CHECK(players.size() == names.size() - 1);
    CHECK(p0->getCoins() == 0);
}

TEST_CASE("Role abilities execute without throwing") {
    cout << "[TEST] Role abilities execute without throwing" << endl;
    Game game(names);
    auto baron = dynamic_cast<Baron*>(game.getPlayers()[2]);
    baron->addCoins(3);
    CHECK_NOTHROW(baron->useAbility(game));
    auto governor = dynamic_cast<Governor*>(game.getPlayers()[0]);
    CHECK_NOTHROW(governor->useAbility(game));
    auto spy = dynamic_cast<Spy*>(game.getPlayers()[1]);
    CHECK_NOTHROW(spy->useAbility(game));
    auto judge = dynamic_cast<Judge*>(game.getPlayers()[4]);
    CHECK_NOTHROW(judge->useAbility(game));
    auto merchant = dynamic_cast<Merchant*>(game.getPlayers()[5]);
    CHECK_NOTHROW(merchant->useAbility(game));
}

TEST_CASE("Game over and winner detection") {
    cout << "[TEST] Game over and winner detection" << endl;
    vector<string> two = {"A","B"};
    Game game(two);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    p0->addCoins(7);
    game.coup(p0, p1);
    CHECK(game.isGameOver(game.getPlayers()));
    CHECK(game.winner() == p0->getName());
}

TEST_CASE("Exception handling flags recoverable errors") {
    cout << "[TEST] Exception handling flags recoverable errors" << endl;
    Game game(names);
    auto p = game.getPlayers()[0];
    // no exception type: gather twice
    game.gather(p);
    bool cont = game.handleException(GatherError("fail"));
    CHECK(cont);
    // unknown exception
    cont = game.handleException(std::runtime_error("x"));
    CHECK(!cont);
}