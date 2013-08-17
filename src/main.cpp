#include <eris/Eris.hpp>
#include <eris/Simulation.hpp>
#include <eris/Position.hpp>
#include "voting/Voter.hpp"
#include <iostream>

using namespace voting;

int main() {
    Eris<Simulation> sim;

//    auto p_left  = sim->createAgent<Party>(Position({-0.5}), -1, 0);
//    auto p_right = sim->createAgent<Party>(Position({0.5}), 0, 1);
    auto v1 = sim->createAgent<Voter>(Position({-0.5}));
    auto v2 = sim->createAgent<Voter>(Position({0.4}));
    auto v3 = sim->createAgent<Voter>(Position({0.0}));

    std::cout << "v1 at: " << v1->position() << "\n";
    std::cout << "v2 at: " << v2->position() << "\n";
    std::cout << "v3 at: " << v3->position() << "\n";

    std::cout << "Adding 1\n";
    v3->addFriend(v1);
    std::cout << "Added 1\n";
    v3->addFriend(v2);
    std::cout << "Added 2\n";
    eris_id_t v3id = v3;

    std::cout << "v3id = " << v3id << "\n";

    for (auto &f: v3->friends()) {
        std::cout << "v3 friend at position " << f.second->position() << "\n";
    }

}
