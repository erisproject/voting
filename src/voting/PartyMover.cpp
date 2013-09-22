#include "voting/PartyMover.hpp"
#include "voting/Election.hpp"
#include <random>
#include <eris/Random.hpp>
#include "voting/Party.hpp"
#include "voting/Poll.hpp"

#include <iostream>

namespace voting {

using eris::Stepper;

PartyMover::PartyMover(eris_id_t party, double step) : party_(party), step_gen_([step]() { return step; }) {}
PartyMover::PartyMover(eris_id_t party, std::function<double()> step_gen) : party_(party), step_gen_(step_gen) {}

void PartyMover::added() {
    dependsOn(party_);
}

void PartyMover::optimize() const {
    move_ = should_move();
}

double PartyMover::should_move() const {
    auto party = simAgent<Party>(party_);

    for (auto &election : simulation()->agentFilter<Election>()) {
        if (election.second->electionPeriod()) {
            // No movement during an election period.
            return 0.0;
        }
    }

    auto pollsters = simulation()->agentFilter<Poll>();
    if (pollsters.empty())
        throw std::runtime_error("PartyMover optimizer failure: simulation has no Poll agent\n");
    auto pollster = pollsters.begin()->second;

    std::unordered_map<double, int> polls;

    double step_size = step_gen_(); 

    // Conduct three polls: a "don't move", one step left, and one step right.
    polls[0.0] = pollster->conductPoll().closest_party[party];
    if (!party->bindingLower())
        polls[-step_size] = pollster->conductPollIf(
                party,
                party->toBoundary(party->position() - Position({ step_size }))).closest_party[party];
    if (!party->bindingUpper())
        polls[step_size] = pollster->conductPollIf(
                party,
                party->toBoundary(party->position() + Position({ step_size }))).closest_party[party];

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

void PartyMover::apply() {
    if (move_ != 0.0) {
        auto party = simAgent<Party>(party_);
        party->moveBy(move_);
    }
}

}
