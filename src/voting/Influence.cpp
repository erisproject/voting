#include "voting/Influence.hpp"
#include "voting/Voter.hpp"
#include <eris/Random.hpp>
#include <random>

namespace voting {
Influence::Influence(eris_id_t voter, double probability, double drift, bool rand_influencees)
    : voter_(voter), prob_(probability), drift_(drift), rand_inflees_(rand_influencees) {}

void Influence::apply() {
    auto voter = simAgent<Voter>(voter_);
    auto friends = voter->friends();
    auto infls = influencees(friends);

    for (auto &inf : infls) {
        if (0) friends.at(inf)->attemptInfluence(voter);
    }
}

const std::list<eris_id_t> Influence::influencees(const std::unordered_map<eris_id_t, SharedMember<Voter>> &friends) {
    auto rng = eris::Random::rng();
    std::bernoulli_distribution flip(prob_);
    std::list<eris_id_t> ret;
    if (rand_inflees_) {
        for (const auto &f : friends) {
            if (flip(rng))
                ret.push_back(f.first);
        }
    }
    else if (flip(rng)) {
        for (const auto &f : friends)
            ret.push_back(f.first);
    }
    return ret;
}

void Influence::postAdvance() {
}

}
