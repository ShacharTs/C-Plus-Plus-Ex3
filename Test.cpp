#include "Game.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace std;

TEST_CASE("Game constructor with predefined players") {
    std::vector<std::string> names = { "Alice", "Bob" };
    coup::Game game(names);  // use test-friendly constructor

    CHECK(names.size() == 2);  // test input size
    // Add more checks as needed...
}
