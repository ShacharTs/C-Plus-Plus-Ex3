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

static const vector<string> names = {"Bob","Alice","Steven","Patrick","Cat","Shachar"};

TEST_CASE("Game initialization and copying") {
    SUBCASE("Initial roles assigned correctly") {
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
    SUBCASE("Copy assignment operator deep copies") {
        Game game1(names);
        Game game2 = game1;
        CHECK(game2.getTurn() == game1.getTurn());
        const auto& p1 = game1.getPlayers();
        const auto& p2 = game2.getPlayers();
        CHECK(p1.size() == p2.size());
        for (size_t i = 0; i < p1.size(); ++i) {
            CHECK(p1[i] != p2[i]);
            CHECK(p1[i]->getName() == p2[i]->getName());
            CHECK(p1[i]->getRole() == p2[i]->getRole());
            CHECK(p1[i]->getCoins() == p2[i]->getCoins());
            p1[i]->addCoins(1);
            CHECK(p1[i]->getCoins() != p2[i]->getCoins());
        }
    }
    SUBCASE("Copy constructor clones players correctly") {
        Game game1(names);
        Game game2(game1);
        const auto& p1 = game1.getPlayers();
        const auto& p2 = game2.getPlayers();
        CHECK(p1.size() == p2.size());
        for (size_t i = 0; i < p1.size(); ++i) {
            CHECK(p1[i] != p2[i]);
            CHECK(p1[i]->getName() == p2[i]->getName());
        }
    }
}

TEST_CASE("Action mechanics") {
    Game game(names);
    auto p0 = game.getPlayers()[0];

    SUBCASE("Gather increases coins and consumes turn") {
        int before = p0->getCoins();
        game.gather(p0);
        CHECK(p0->getCoins() == before + 1);
        CHECK_THROWS_AS(game.gather(p0), TurnError);
    }
    SUBCASE("Tax increases coins and consumes turn") {
        game.nextTurn();
        auto p1 = game.getPlayers()[1];
        int before = p1->getCoins();
        game.tax(p1);
        CHECK(p1->getCoins() == before + 2);
        CHECK_THROWS_AS(game.tax(p1), TurnError);
    }
    SUBCASE("Bribe grants extra turn and cost deducted") {
        auto baron = game.getPlayers()[2];
        baron->addCoins(5);
        game.bribe(baron);
        CHECK(baron->getNumOfTurns() == 2);
        CHECK(baron->getCoins() == 1);
    }
    SUBCASE("Bribe without enough coins throws") {
        auto baron = game.getPlayers()[2];
        CHECK_THROWS_AS(game.bribe(baron), CoinsError);
    }
    SUBCASE("Arrest steals coin and records last arrested") {
        auto p1 = game.getPlayers()[1];
        p0->addCoins(1);
        p1->addCoins(1);
        game.arrest(p0, p1);
        CHECK(p0->getCoins() == 2);
        CHECK(p1->getCoins() == 0);
        CHECK_THROWS_AS(game.arrest(p0, p0), SelfError);
    }
}

TEST_CASE("Coup and sanctions") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];

    SUBCASE("Sanction deducts cost and disables target") {
        p0->addCoins(3);
        game.sanction(p0, p1);
        CHECK(!p1->isGatherAllow());
        CHECK(!p1->isTaxAllow());
        CHECK(p0->getCoins() == 0);
    }
    SUBCASE("Sanction self throws") {
        p0->addCoins(3);
        CHECK_THROWS_AS(game.sanction(p0, p0), SelfError);
    }
    SUBCASE("Sanction insufficient funds throws") {
        CHECK_THROWS_AS(game.sanction(p0, p1), CoinsError);
    }
    SUBCASE("Coup removes target and costs coins") {
        p0->addCoins(7);
        game.coup(p0, p1);
        CHECK(game.getPlayers().size() == names.size() - 1);
        CHECK(p0->getCoins() == 0);
    }
    SUBCASE("Coup self throws") {
        p0->addCoins(7);
        CHECK_THROWS_AS(game.coup(p0, p0), SelfError);
    }
    SUBCASE("Coup insufficient funds throws") {
        CHECK_THROWS_AS(game.coup(p0, p1), CoinsError);
    }
    SUBCASE("General cannot coup with exactly 5 coins") {
        Player* gen = nullptr;
        for (auto p : game.getPlayers()) if (p->getRole() == Role::General) gen = p;
        REQUIRE(gen != nullptr);
        gen->addCoins(5);
        auto victim = game.getListOfTargetPlayers(gen).front();
        CHECK_THROWS_AS(game.coup(gen, victim), CoinsError);
        CHECK(gen->getCoins() == 5);
    }
}

TEST_CASE("Turn flow utilities") {
    Game game(names);
    auto p0 = game.getPlayers()[0];

    SUBCASE("skipTurn consumes extra turn first") {
        p0->addExtraTurn();
        REQUIRE(p0->hasExtraTurn());
        game.skipTurn(p0);
        REQUIRE(p0->hasExtraTurn());
        game.skipTurn(p0);
        REQUIRE(!p0->hasExtraTurn());
    }
    SUBCASE("skipTurn without extra-turn consumes default turn") {
        game.skipTurn(p0);
        CHECK(!game.currentPlayerHasTurn());
    }
    SUBCASE("advanceTurnIfNeeded advances when no extra-turn") {
        int start = game.getTurn();
        game.tax(p0);
        game.advanceTurnIfNeeded();
        CHECK(game.getTurn() == (start + 1) % names.size());
    }
    SUBCASE("advanceTurnIfNeeded holds when extra-turn") {
        p0->addExtraTurn();
        int start = game.getTurn();
        game.advanceTurnIfNeeded();
        CHECK(game.getTurn() == start);
    }
    SUBCASE("handleBlock deducts cost and consumes turn") {
        auto p1 = game.getPlayers()[1];
        p1->addCoins(3);
        bool blocked = game.handleBlock(p1, true, "Test", 2);
        CHECK(blocked);
        CHECK(p1->getCoins() == 1);
        CHECK(!game.currentPlayerHasTurn());
    }
    SUBCASE("handleBlock returns false if didBlock false") {
        auto p1 = game.getPlayers()[1];
        p1->addCoins(5);
        CHECK_FALSE(game.handleBlock(p1, false, "Nope", 3));
    }
}

TEST_CASE("playerPayAfterBlock effects") {
    Game game(names);
    auto gov = dynamic_cast<Governor*>(game.getPlayers()[0]);
    auto judge = dynamic_cast<Judge*>(game.getPlayers()[4]);
    auto spy   = dynamic_cast<Spy*>(game.getPlayers()[1]);
    Player* gen = nullptr;
    for (auto p : game.getPlayers()) if (p->getRole() == Role::General) gen = p;
    REQUIRE(gov != nullptr);
    REQUIRE(judge != nullptr);
    REQUIRE(spy != nullptr);
    REQUIRE(gen != nullptr);

    SUBCASE("Governor blocks Tax disables canTax") {
        gov->canTax = true;
        game.playerPayAfterBlock(gov, Role::Governor);
        CHECK_FALSE(gov->canTax);
    }
    SUBCASE("Judge blocks Bribe deducts 4 coins") {
        judge->addCoins(5);
        int before = judge->getCoins();
        game.playerPayAfterBlock(judge, Role::Judge);
        CHECK(judge->getCoins() == before - 4);
    }
    SUBCASE("Spy blocks Arrest disables canArrest") {
        spy->canArrest = true;
        game.playerPayAfterBlock(spy, Role::Spy);
        CHECK_FALSE(spy->canArrest);
    }
    SUBCASE("General blocks Coup pays 5 coins") {
        gen->addCoins(6);
        auto current = game.getPlayers()[game.getTurn()];
        current->addCoins(7);
        int before = gen->getCoins();
        game.playerPayAfterBlock(gen, Role::General);
        CHECK(gen->getCoins() == before - 5);
    }
    SUBCASE("Invalid block role throws CoinsError") {
        auto baron = dynamic_cast<Baron*>(game.getPlayers()[2]);
        CHECK_THROWS_AS(game.playerPayAfterBlock(baron, Role::Baron), CoinsError);
    }
}

TEST_CASE("Role abilities execute") {
    Game game(names);
    auto baron     = dynamic_cast<Baron*>(game.getPlayers()[2]);
    auto governor  = dynamic_cast<Governor*>(game.getPlayers()[0]);
    auto spyRole   = dynamic_cast<Spy*>(game.getPlayers()[1]);
    auto judgeRole = dynamic_cast<Judge*>(game.getPlayers()[4]);
    auto merchant  = dynamic_cast<Merchant*>(game.getPlayers()[5]);
    REQUIRE(baron != nullptr);
    REQUIRE(governor != nullptr);
    REQUIRE(spyRole != nullptr);
    REQUIRE(judgeRole != nullptr);
    REQUIRE(merchant != nullptr);

    SUBCASE("Baron useAbility exchanges coins") {
        baron->addCoins(3);
        int before = baron->getCoins();
        baron->useAbility(game);
        CHECK(baron->getCoins() == before - 3 + 6);
    }
    SUBCASE("Governor use taxes") {
        int before = governor->getCoins();
        REQUIRE_NOTHROW(governor->tax());
        CHECK(governor->getCoins() == before + 3);
    }
    SUBCASE("Spy ability grants extra turn") {
        int before = spyRole->getNumOfTurns();
        spyRole->useAbility(game);
        CHECK(spyRole->getNumOfTurns() == before + 1);
    }
    SUBCASE("Judge passive penalizes target") {
        auto victim = game.getPlayers()[0];
        victim->addCoins(2);
        judgeRole->passiveAbility(victim);
        CHECK(victim->getCoins() == 1);
    }
    SUBCASE("Merchant passive gives extra coin each turn") {
        int before = merchant->getCoins();
        merchant->passiveAbility();
        CHECK(merchant->getCoins() == before + 1);
    }
    SUBCASE("Multiple bribes accumulate extra-turns") {
        judgeRole->addCoins(8);
        CHECK(judgeRole->getNumOfTurns() == 1);
        game.bribe(judgeRole);
        game.nextTurn();
        CHECK(judgeRole->getNumOfTurns() == 2);
        game.bribe(judgeRole);
        game.nextTurn();
        CHECK(judgeRole->getNumOfTurns() == 3);
    }
    SUBCASE("skipTurn consumes only one extra-turn") {
        judgeRole->addCoins(8);
        game.bribe(judgeRole);
        game.skipTurn(judgeRole);
        CHECK(judgeRole->getNumOfTurns() == 1);
    }
}

TEST_CASE("Game over and winner detection") {
    SUBCASE("Winner after last coup") {
        vector<string> two = {"A","B"};
        Game game(two);
        auto p0 = game.getPlayers()[0];
        auto p1 = game.getPlayers()[1];
        p0->addCoins(7);
        game.coup(p0, p1);
        CHECK(game.isGameOver(game.getPlayers()));
        CHECK(game.winner() == p0->getName());
    }
    SUBCASE("Calling winner too early throws") {
        Game game({"A","B","C"});
        CHECK_THROWS_AS(game.winner(), GameOverError);
    }
}

TEST_CASE("Utility methods") {
    Game game(names);
    auto p0 = game.getPlayers()[0];

    SUBCASE("getListOfTargetPlayers excludes current") {
        auto list = game.getListOfTargetPlayers(p0);
        CHECK(list.size() == names.size() - 1);
        for (auto* p : list) CHECK(p != p0);
    }
    SUBCASE("nextTurn resets gather/tax flags") {
        game.gather(p0);
        for (size_t i = 0; i < names.size(); ++i) game.nextTurn();
        CHECK(p0->isGatherAllow());
        CHECK(p0->isTaxAllow());
    }
}

TEST_CASE("Double-sanction allowed, still effective") {
    Game game(names);
    auto p0 = game.getPlayers()[0], p1 = game.getPlayers()[1];
    p0->addCoins(3);
    game.sanction(p0, p1);
    game.nextTurn();
    CHECK_THROWS_AS(game.gather(p1), GatherError);
    CHECK_THROWS_AS(game.tax(p1), TaxError);
    p0->addCoins(3);
    game.sanction(p0, p1);
    CHECK(!p1->isGatherAllow());
}

TEST_CASE("Gather/tax blocked by sanction throws GatherError/TaxError") {
    Game game(names);
    auto p0 = game.getPlayers()[0], p1 = game.getPlayers()[1];
    p0->addCoins(3);
    game.sanction(p0, p1);
    game.nextTurn();
    CHECK_THROWS_AS(game.gather(p1), GatherError);
    CHECK_THROWS_AS(game.tax(p1), TaxError);
}

TEST_CASE("Sanction lifts after next full cycle") {
    Game game(names);
    auto p0 = game.getPlayers()[0], p1 = game.getPlayers()[1];
    p0->addCoins(3);
    game.sanction(p0, p1);
    game.nextTurn();
    CHECK_THROWS_AS(game.gather(p1), GatherError);
    game.skipTurn(p1);
    CHECK(p1->isGatherAllow());
    for (auto p : game.getPlayers()) if (p != p1) game.skipTurn(p);
    CHECK_THROWS(game.gather(p1));
}

TEST_CASE("Double-tax not allowed in one turn") {
    Game game(names);
    auto p = game.getPlayers()[0];
    game.tax(p);
    CHECK_THROWS_AS(game.tax(p), TurnError);
}

TEST_CASE("Turn advances after player removed via coup") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    // grab the name *before* p1 is destroyed
    std::string victimName = p1->getName();

    p0->addCoins(7);
    game.coup(p0, p1);
    game.advanceTurnIfNeeded();  // Ensure p1 is skipped

    // now compare against the saved string
    CHECK(game.turn() != victimName);
}



TEST_CASE("Player Basic Functionality using Governor") {
    Player* p = new Governor("TEST_NAME");
    CHECK(p->getName() == "TEST_NAME");
    CHECK(p->getRole() == Role::Governor);
    CHECK(p->getCoins() == 0);
    delete p;
}

TEST_CASE("Player Coin Management using Governor") {
    Player* p = new Governor("Bob");

    SUBCASE("Add coins") {
        CHECK_NOTHROW(p->addCoins(5));
        CHECK(p->getCoins() == 5);
        CHECK_THROWS_AS(p->addCoins(-5), CoinsError);
        CHECK_NOTHROW(p->addCoins(0));
        CHECK(p->getCoins() == 5);
    }

    SUBCASE("Remove coins") {
        p->addCoins(10);
        CHECK_NOTHROW(p->removeCoins(3));
        CHECK(p->getCoins() == 7);
        CHECK_THROWS(p->removeCoins(10));
        CHECK(p->getCoins() == 7);
        CHECK_THROWS_AS(p->removeCoins(-5), CoinsError);
        CHECK_NOTHROW(p->removeCoins(0));
        CHECK(p->getCoins() == 7);
    }

    delete p;
}

TEST_CASE("Player Gather Action using Governor") {
    Governor* p = new Governor("John");
    SUBCASE("Successful gather") {
        int before = p->getCoins();
        CHECK_NOTHROW(p->gather());
        CHECK(p->getCoins() == before + 1);
    }

    SUBCASE("Gather blocked") {
        p->canGather = false;
        CHECK_THROWS_AS(p->gather(), GatherError);
    }

    delete p;
}

TEST_CASE("Player Tax Action using Governor") {
    Governor* p = new Governor("John");
    SUBCASE("Successful tax") {
        CHECK_NOTHROW(p->tax());
        CHECK(p->getCoins() == 3);
    }

    SUBCASE("Tax blocked") {
        p->canTax = false;
        CHECK_THROWS_AS(p->tax(), TaxError);
    }

    delete p;
}

TEST_CASE("Player Bribe Action using Judge") {
    Judge* p = new Judge("John");

    SUBCASE("Grant extra turn without cost check") {
        int before = p->getNumOfTurns();
        CHECK_NOTHROW(p->bribe());
        CHECK(p->getNumOfTurns() == before + 1);
    }

    delete p;
}

TEST_CASE("Player Arrest Action using Spy") {
    Spy* p1 = new Spy("John");
    Player* p2 = new Governor("Doe");

    SUBCASE("Successful arrest steals coin") {

        p2->addCoins(1);
        int before1 = p1->getCoins();
        CHECK_NOTHROW(p1->arrest(p2));
        CHECK(p1->getCoins() == before1 + 1);
    }

    SUBCASE("Self arrest throws") {

        CHECK_THROWS_AS(p1->arrest(p1), SelfError);
    }

    delete p1; delete p2;
}

TEST_CASE("Player Sanction Action using Baron") {
    Baron* p1 = new Baron("John");
    Player* p2 = new Governor("Doe");

    SUBCASE("Successful sanction blocks gather/tax on target") {

        p1->addCoins(3);
        CHECK_NOTHROW(p1->sanction(p2));
        CHECK_FALSE(p2->isGatherAllow());
        CHECK_FALSE(p2->isTaxAllow());
    }

    SUBCASE("Sanction self throws") {

        CHECK_THROWS_AS(p1->sanction(p1), SelfError);
    }

    delete p1; delete p2;
}

TEST_CASE("Player Coup Action using Game and General") {
    vector<string> names = {"A","B","C","John","Doe"};
    Game game(names);
    auto players = game.getPlayers();
    General* gen = nullptr;
    for (auto p : players) {
        if (p->getRole() == Role::General) { gen = dynamic_cast<General*>(p); break; }
    }
    REQUIRE(gen != nullptr);
    Player* victim = nullptr;
    for (auto p : players) {
        if (p != gen) { victim = p; break; }
    }
    REQUIRE(victim != nullptr);

    SUBCASE("Successful coup costs coins") {
        gen->addCoins(7);
        CHECK_NOTHROW(game.coup(gen, victim));
        CHECK(gen->getCoins() == 0);
        CHECK(game.getPlayers().end() == std::find(game.getPlayers().begin(), game.getPlayers().end(), victim));
    }

    SUBCASE("Coup without coins throws") {
        CHECK_THROWS_AS(game.coup(gen, victim), CoinsError);
    }
}

TEST_CASE("Player End Turn Clears Debuffs using Merchant") {
    Merchant* p = new Merchant("John");
    p->canGather = p->canTax = p->canArrest = false;
    CHECK_NOTHROW(p->playerUsedTurn());
    CHECK(p->isGatherAllow());
    CHECK(p->isTaxAllow());
    CHECK(p->isArrestAllow());
    delete p;
}

TEST_CASE("Action out of turn throws TaxError") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    game.gather(p0);
    game.skipTurn(p0); // turn to p1
    game.skipTurn(p1); // skip p1's turn
    CHECK_THROWS_AS(game.tax(p1), TurnError);
}

TEST_CASE("Baron useAbility updates coin count correctly") {
    Game game(names);
    auto baron = dynamic_cast<Baron*>(game.getPlayers()[2]);
    CHECK(baron->getCoins() == 0);
    baron->addCoins(3);
    CHECK(baron->getCoins() == 3);
    int before = baron->getCoins();
    baron->useAbility(game);
    CHECK(baron->getCoins() == before - 3 + 6);
}

TEST_CASE("Coup removes player and getListOfTargetPlayers skips victim; nextTurn wraps") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    p0->addCoins(7);
    game.coup(p0, p1);

    auto targets = game.getListOfTargetPlayers(p0);
    CHECK(std::find(targets.begin(), targets.end(), p1) == targets.end());

    size_t n = game.getPlayers().size();
    for(size_t i=0; i< n * 2; ++i) {
        game.nextTurn();
        auto cur = game.getPlayers()[game.getTurn()];
        REQUIRE(cur != p1);
    }
}

TEST_CASE("Coup with insufficient funds throws") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    // p0 has 0 coins, coup costs 7
    CHECK_THROWS_AS(game.coup(p0, p1), CoinsError);
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

TEST_CASE("Game copy assignment operator performs deep copy") {
    Game game1(names);
    Game game2 = game1;

    // Validate scalar data
    CHECK(game2.getTurn() == game1.getTurn());

    const auto& players1 = game1.getPlayers();
    const auto& players2 = game2.getPlayers();
    CHECK(players1.size() == players2.size());

    for (size_t i = 0; i < players1.size(); ++i) {
        auto* p1 = players1[i];
        auto* p2 = players2[i];

        // Ensure they are deep copied (not same pointer)
        CHECK(p1 != p2);

        // Validate key data copied
        CHECK(p1->getName() == p2->getName());
        CHECK(p1->getRole() == p2->getRole());
        CHECK(p1->getCoins() == p2->getCoins());

        // Modify original, check no side effect
        p1->addCoins(10);
        CHECK(p1->getCoins() != p2->getCoins());
    }
}

TEST_CASE("Game copy constructor clones players correctly") {
    Game game1(names);
    Game game2 = game1;  // Copy

    CHECK(game2.getPlayers().size() == game1.getPlayers().size());
    for (size_t i = 0; i < names.size(); ++i) {
        CHECK(game2.getPlayers()[i] != game1.getPlayers()[i]);  // Deep copy
        CHECK(game2.getPlayers()[i]->getName() == game1.getPlayers()[i]->getName());
    }
}

TEST_CASE("Game over returns correct winner") {
    vector<string> two = {"A", "B"};
    Game game(two);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    p0->addCoins(7);
    game.coup(p0, p1);        // eliminate p1
    CHECK(game.isGameOver(game.getPlayers()));
    CHECK(game.winner() == p0->getName());
}

TEST_CASE("Gather increase coins and consume turn") {
    cout << "[TEST] Gather increase coins and consume turn" << endl;
    Game game(names);
    auto p = game.getPlayers()[0];
    int before = p->getCoins();
    game.gather(p);
    CHECK(p->getCoins() == before + 1);
    // gather not allowed twice
    CHECK_THROWS_AS(game.gather(p), TurnError);
}

TEST_CASE("General blocks Coup - pays 5 coins") {
    Game game(names);
    auto gen = dynamic_cast<General*>(game.getPlayers()[3]);
    REQUIRE(gen != nullptr);
    gen->addCoins(6);
    Player* current = game.getPlayers()[game.getTurn()];
    current->addCoins(7);
    game.playerPayAfterBlock(gen, Role::General);
    CHECK(gen->getCoins() == 1);
}

TEST_CASE("Governor blocks Tax - disables tax ability") {
    Game game(names);
    auto gov = dynamic_cast<Governor*>(game.getPlayers()[0]);
    REQUIRE(gov != nullptr);

    // Ensure canTax is true before
    gov->canTax = true;
    game.playerPayAfterBlock(gov, Role::Governor);
    CHECK_FALSE(gov->canTax); // Tax ability should now be disabled
}

TEST_CASE("Invalid block role throws error") {
    Game game(names);
    auto baron = dynamic_cast<Baron*>(game.getPlayers()[2]);
    REQUIRE(baron != nullptr);

    CHECK_THROWS_AS(game.playerPayAfterBlock(baron, Role::Baron), CoinsError);
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

TEST_CASE("Sanction against Judge applies retaliation penalty") {
    Game game(names);
    auto p0    = game.getPlayers()[0];
    auto judge = game.getPlayers()[4]; // Index 4 is Judge
    p0->addCoins(6);                   // Enough for sanction + retaliation
    game.sanction(p0, judge);
    // p0 paid 3 for sanction, then lost 1 more to judge’s passiveAbility
    CHECK(p0->getCoins() == 2);
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

TEST_CASE("Sanction with insufficient funds throws") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    // p0 has 0 coins
    CHECK_THROWS_AS(game.sanction(p0, p1), CoinsError);
}

TEST_CASE("Self-coup should throw") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    p0->addCoins(7);
    CHECK_THROWS_AS(game.coup(p0, p0), SelfError);
}
TEST_CASE("Self-sanction should throw") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    p0->addCoins(3);
    CHECK_THROWS_AS(game.sanction(p0, p0), SelfError);
}

TEST_CASE("Spy blocks Arrest - disables arrest ability") {
    Game game(names);
    auto spy = dynamic_cast<Spy*>(game.getPlayers()[1]);
    REQUIRE(spy != nullptr);

    spy->canArrest = true;
    game.playerPayAfterBlock(spy, Role::Spy);
    CHECK_FALSE(spy->canArrest);
}

TEST_CASE("Tax increase coins and consume turn") {
    cout << "[TEST] Tax increase coins and consume turn" << endl;
    Game game(names);
    auto p = game.getPlayers()[0];
    int before = p->getCoins();
    game.nextTurn();
    auto p1 = game.getPlayers()[1];
    before = p1->getCoins();
    game.tax(p1);
    CHECK(p1->getCoins() == before + 2);
}

TEST_CASE("advanceTurnIfNeeded advances turn when no extra-turn") {
    Game game(names);
    int start = game.getTurn();
    auto p0 = game.getPlayers()[0];
    game.tax(p0);
    game.advanceTurnIfNeeded();
    CHECK(game.getTurn() == (start + 1) % names.size());
}

TEST_CASE("advanceTurnIfNeeded with extra-turn does not advance") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    p0->addExtraTurn();               // Grant an extra turn
    int start = game.getTurn();
    game.advanceTurnIfNeeded();
    CHECK(game.getTurn() == start);
}

TEST_CASE("clone() returns correct dynamic type") {
    Spy original("SpyMan");
    Player* copy = original.clone();
    CHECK(dynamic_cast<Spy*>(copy) != nullptr);
    delete copy;
}

TEST_CASE("handleBlock deducts cost & consumes turn") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    auto p1 = game.getPlayers()[1];
    p1->addCoins(3);
    int before = p1->getCoins();
    int turnIdx = game.getTurn();
    bool blocked = game.handleBlock(p1, true, "TestAction", 2);
    CHECK(blocked);
    CHECK(p1->getCoins() == before - 2);
    // turn consumed: nextTurn would have advanced, so currentPlayerHasTurn() is false
    CHECK(!game.currentPlayerHasTurn());
}

TEST_CASE("handleBlock on invalid action returns false and no side-effects") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    bool did = game.handleBlock(p0, false, "NotValidAction", 0);
    CHECK(did == false);
    CHECK(p0->getCoins() == 0);
    CHECK(game.getTurn() == 0);
}
TEST_CASE("handleBlock returns false if didBlock is false") {
    Game game(names);
    auto p1 = game.getPlayers()[1];
    p1->addCoins(5);
    CHECK_FALSE(game.handleBlock(p1, false, "Nope", 3));
}

TEST_CASE("skipTurn consumes extra turn") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    p0->addExtraTurn();
    CHECK(p0->hasExtraTurn());
    game.skipTurn(p0);
    CHECK(p0->hasExtraTurn());
    game.skipTurn(p0);
    CHECK(!p0->hasExtraTurn());
}

TEST_CASE("skipTurn without extra-turn consumes turn") {
    Game game(names);
    auto p0 = game.getPlayers()[0];
    game.skipTurn(p0);
    CHECK(!game.currentPlayerHasTurn());
}