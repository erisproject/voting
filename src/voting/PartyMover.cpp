#include "voting/PartyMover.hpp"
#include "voting/Party.hpp"
#include "voting/Poll.hpp"

namespace voting {

using eris::Stepper;

PartyMover::PartyMover(
        eris_id_t party,
        double initial_step,
        double increase_count,
        double min_step)
    : InterStepper(Stepper(initial_step, increase_count, min_step, true)), party_(party) {}

void PartyMover::added() {
    dependsOn(party_);
}

bool PartyMover::should_increase() const {
    auto party = simAgent<Party>(party_);

    if (not party->pollster)
        party->pollster = simulation()->agentFilter().begin()->first;

    auto pollster = simAgent<Poll>(party->pollster);

    if (party->bindingLower()) return true;
    if (party->bindingUpper()) return false;
    if (pollster->allResults().size() < 2) return party->position()[0] >= 0; // No information; move away from 0
    auto curr_poll = pollster->results(0);
    auto prev_poll = pollster->results(-1);
    if (curr_poll.closest_party[party] > prev_poll.closest_party[party])
        // Whatever we did last time apparently helped, keep going
        return stepper_.prev_up;
    else
        // Last time's move appears to not have helped; reverse direction
        return !stepper_.prev_up;
}

}
