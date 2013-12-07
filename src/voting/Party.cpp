#include "voting/Party.hpp"
#include "voting/Election.hpp"
#include "voting/Poll.hpp"
#include <eris/Random.hpp>

namespace voting {

void Party::interOptimize() {
    move_ = should_move();
}

double Party::should_move() {

    for (auto &election : simulation()->agents<Election>()) {
        if (election->electionPeriod()) {
            // No movement during an election period.
            return 0.0;
        }
    }

    auto pollsters = simulation()->agents<Poll>();
    if (pollsters.empty())
        throw std::runtime_error("Party optimization failure: simulation has no Poll agent\n");
    auto pollster = pollsters[0];

    std::unordered_map<double, int> polls;

    double step_size = step_dist_(eris::Random::rng()); 

    // Conduct three polls: a "don't move", one step left, and one step right.
    polls[0.0] = pollster->conductPoll().closest_party[*this];
    if (!bindingLower())
        polls[-step_size] = pollster->conductPollIf(
                *this,
                toBoundary(position() - Position({ step_size }))).closest_party[*this];
    if (!bindingUpper())
        polls[step_size] = pollster->conductPollIf(
                *this,
                toBoundary(position() + Position({ step_size }))).closest_party[*this];

    double highest_step = 0;
    int highest = -1;
    int num_highest = 0;
    for (auto &poll : polls) {
        if (poll.second > highest) {
            highest = poll.second;
            highest_step = poll.first;
            num_highest = 1;
        }
        else if (poll.second == highest) {
            num_highest++;
            if (std::uniform_int_distribution<int>(1, num_highest)(eris::Random::rng()) == 1) {
                highest_step = poll.first;
            }
        }
    }

    return highest_step;
}

void Party::interApply() {
    if (move_ != 0.0)
        moveBy({move_});
}

}
