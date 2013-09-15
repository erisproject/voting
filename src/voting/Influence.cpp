#include "voting/Influence.hpp"
#include "voting/Voter.hpp"
#include <eris/Random.hpp>
#include <random>
#include <iostream>

namespace voting {
Influence::Influence(eris_id_t voter, double probability, double drift, bool influence_all)
    : voter_(voter), prob_(probability), drift_(drift), infl_all_(influence_all) {}

void Influence::apply() {
    auto voter = simAgent<Voter>(voter_);
    auto friends = voter->friends();
    auto infls = influencees(friends);
//    std::cout << "Voter " << voter << " applying influence to " << infls.size() << " (of " << friends.size() << ") friends\n";

    for (auto &inf : infls) {
        inf.second->attemptInfluence(voter, drift_);
    }
}

const std::unordered_map<eris_id_t, SharedMember<Voter>> Influence::influencees(const std::unordered_map<eris_id_t, SharedMember<Voter>> &friends) {
    auto &rng = eris::Random::rng();
    std::bernoulli_distribution flip(prob_);
    std::unordered_map<eris_id_t, SharedMember<Voter>> ret;
    if (not infl_all_) {
        for (const auto &f : friends) {
            if (flip(rng))
                ret.insert(f);
        }
    }
    else if (flip(rng)) {
        return friends;
    }
    return ret;
}

}
