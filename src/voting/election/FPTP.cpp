#include "voting/election/FPTP.hpp"

namespace voting {

eris_id_t FPTP::election() {
    if (!electionPeriod()) throw std::runtime_error("election() called when not in an election period");

    // Already had an election
    if (winner_ > 0) return winner_;

    auto parties = simulation()->agents<Party>();
    if (parties.empty()) throw std::runtime_error("Cannot conduct an election without any parties");
    auto voters = simulation()->agents<Voter>();
    if (voters.empty()) throw std::runtime_error("Cannot conduct an election without any voters");

    votes_.clear();
    
    for (auto &p : parties)
        votes_[p] = 0;

    for (auto &v : voters) {
        eris_id_t closest = 0;
        double min_dist = 0.0;
        int identical = 0;
        for (auto &p : parties) {
            double dist = v->distance(p);
            if (closest == 0 or dist < min_dist) {
                closest = p;
                min_dist = dist;
                identical = 1;
            }
            else if (dist == min_dist) {
                // Found a party exactly the same distance away; we need to change randomly, such
                // that each equal-distance party has an equal chance of being chosen
                if (++identical > 1) {
                    if (std::bernoulli_distribution(1.0 / identical)(eris::Random::rng())) {
                        closest = p;
                    }
                }
            }
        }

        votes_[closest]++;
    }

    int most = -1, most_count = 0;
    for (auto &p : votes_) {
        if (p.second > most) {
            winner_ = p.first;
            most = p.second;
            most_count = 1;
        }
        else if (p.second == most) {
            most_count++;
            // Decide a tie with randomization.  Change to this party with probability 1/T, where T
            // is the number of ties found so far.  (Thus each party will have an equal probability
            // of winning).
            if (std::bernoulli_distribution(1.0 / most_count)(eris::Random::rng())) {
                winner_ = p.first;
            }
        }
    }

    return winner_;
}


const std::unordered_map<eris_id_t, int>& FPTP::votes() {
    if (not winner_ > 0) election();

    return votes_;
}

void FPTP::interAdvance() {
    Election::interAdvance();
    winner_ = 0;
    votes_.clear();
}

}

