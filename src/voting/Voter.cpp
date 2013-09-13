#include "voting/Voter.hpp"
#include "voting/Party.hpp"
#include <eris/Random.hpp>
#include <iostream>

namespace voting {

bool Voter::isFriend(eris_id_t voter) const {
    return friends().count(voter) > 0;
}

void Voter::addFriend(eris_id_t voter) {
    auto v = simAgent<Voter>(voter);
    std::cout << "got the voter\n";
    auto pair = std::make_pair(voter, v);
    std::cout << "made the pair\n";
    friends_.insert(pair);
    std::cout << "inserted it\n";
}

void Voter::removeFriend(eris_id_t voter) {
    friends_.erase(voter);
}


const std::unordered_map<eris_id_t, SharedMember<Voter>>& Voter::friends() const {
    return friends_;
}

double Voter::friendDistance(eris_id_t voter) const {
    if (not isFriend(voter)) return -1;

    return position().distance(friends_.at(voter)->position());
}

double Voter::conviction() const {
    double min_dist = 2.0;
    for (auto &p : simulation()->agentFilter<Party>()) {
        double pdist = position().distance(p.second->position());
        if (pdist < min_dist) min_dist = pdist;
    }
    return 1.0 - min_dist/2.0;
}

bool Voter::attemptInfluence(const Voter &by) {
    double my_conv = conviction();
    double their_conv = by.conviction();

    double p = influenceProbability(my_conv, their_conv);

    bool success;
    if (p >= 1)
        success = true;
    else if (p <= 0)
        success = false;
    else
        success = std::bernoulli_distribution(p)(eris::Random::rng());

    if (success) {
        if (!natural_pos_) {
            // This is the first time the voter has moved, so make a note of the voter's home
            // position (so that we can drift back to it).
            natural_pos_ = std::unique_ptr<Position>(new Position(position()));
        }

        moveTo(by.position());
    }

    return success;
}

double Voter::influenceProbability(double my_conv, double their_conv) const {
    if (my_conv < 0.0) my_conv = 0.0;
    else if (my_conv > 1.0) my_conv = 1.0;

    if (their_conv < 0.0) their_conv = 0.0;
    else if (their_conv > 1.0) their_conv = 1.0;

    return (1.0 + their_conv - my_conv) / 2.0;
}


const Position Voter::naturalPosition() const {
    if (natural_pos_) return *natural_pos_;
    return position();
}

}
