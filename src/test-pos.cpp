#include "voting/Position.hpp"
#include <iostream>

using namespace voting;

int main() {
    Position p({1.0/4.0, 0, -44.32716494141});
    Position origin({0,0,0});

    Position phalf = origin.mean(p);
    Position pneg = origin.mean(p, -1);

    std::cout << "p     =" << p << "\n";
    std::cout << "origin=" << origin << "\n";
    std::cout << "phalf =" << phalf << "\n";
    std::cout << "pneg  =" << pneg << "\n";
}
