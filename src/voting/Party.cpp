#include "voting/Party.hpp"
#include "voting/PartyMover.hpp"
#include <eris/Random.hpp>
#include <random>
#include <iostream>

namespace voting {

void Party::added() {
    auto &step_dist = step_dist_;
    simulation()->createInterOpt<PartyMover>(*this, [&step_dist]() -> double { return step_dist(eris::Random::rng()); });
}

}
