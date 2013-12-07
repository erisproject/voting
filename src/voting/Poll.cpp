#include "voting/Poll.hpp"
#include "voting/Election.hpp"
#include "voting/Party.hpp"
#include "voting/Voter.hpp"
#include <eris/Random.hpp>
#include <iostream>
#include <random>

namespace voting {


Poll::poll Poll::conductPollIf(eris_id_t party_id, Position try_pos) {
    // First check to make sure this isn't an election period:
    for (auto &election : simulation()->agents<Election>()) {
        if (election->electionPeriod())
            throw std::runtime_error("Cannot poll during an election period");
    }
    auto parties = simulation()->agents<Party>();
    auto voters = simulation()->agents<Voter>();

    Poll::poll new_poll;
    for (auto &p : parties)
        new_poll.closest_party[p] = 0;

    for (auto &v : voters) {
        eris_id_t closest = 0;
        double min_dist = 0.0;
        int identical = 0;
        for (auto &p : parties) {
            const Position &pos = p == party_id ? try_pos : p->position();
            double dist = v->position().distance(pos);
            if (closest == 0 or dist < min_dist) {
                closest = p;
                min_dist = dist;
                identical = 1;
            }
            else if (dist == min_dist) {
                // Found a party exactly the same distance away; we need to change randomly, such
                // that each equal-distance party has an equal chance of being chosen
                if (++identical > 1) {
                    if (std::uniform_int_distribution<int>(1, identical)(eris::Random::rng()) == 1) {
                        closest = p;
                    }
                }
            }
        }

        new_poll.closest_party[closest]++;
    }

    return new_poll;
}

Poll::poll Poll::conductPoll() {
    return conductPollIf(0, Position({0}));
}

}
