#include "voting/Poll.hpp"
#include "voting/Party.hpp"
#include "voting/Voter.hpp"

namespace voting {

void Poll::advance() {
    auto parties = simulation()->agentFilter<Party>();
    auto voters = simulation()->agentFilter<Voter>();

    Poll::poll new_poll;
    for (auto &p : parties)
        new_poll.closest_party[p.first] = 0;

    for (auto &v : voters) {
        eris_id_t closest = 0;
        double min_dist = 0.0;
        for (auto &p : parties) {
            double dist = v.second->position().distance(p.second->position());
            if (closest == 0 or dist < min_dist) {
                closest = p.first;
                min_dist = dist;
            }
        }

        new_poll.closest_party[closest]++;
    }

    results_.push_back(std::move(new_poll));
}

const Poll::poll& Poll::results(int n) {
    return results_.at(results_.size() - 1 + n);
}

const std::vector<Poll::poll>& Poll::allResults() {
    return results_;
}

}
