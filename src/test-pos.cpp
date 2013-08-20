#include <eris/Position.hpp>
#include <iostream>

using namespace eris;

int main() {
    Position p({1.0/4.0, 0, -44.32716494141});
    Position origin({0,0,0});

    Position phalf = origin.mean(p);
    Position pneg = origin.mean(p, -1);

    std::cout << "p     =" << p << "\n";
    std::cout << "origin=" << origin << "\n";
    std::cout << "phalf =" << phalf << "\n";
    std::cout << "pneg  =" << pneg << "\n";

    Position origin1 {0};
    Position origin2 {0,0};
    Position origin3 {0,0,0};
    Position origin4 {0,0,0,0};

    std::cout << "origin=" << origin1 << "\n";
    std::cout << "origin=" << origin2 << "\n";
    std::cout << "origin=" << origin3 << "\n";
    std::cout << "origin=" << origin4 << "\n";
}
