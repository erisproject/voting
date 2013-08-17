#include "voting/Voter.hpp"
#include <iostream>

namespace voting {

bool Voter::isFriend(eris_id_t voter) {
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


const std::unordered_map<eris_id_t, SharedMember<Voter>>& Voter::friends() {
    return friends_;
}

double Voter::friendDistance(eris_id_t voter) {
    if (not isFriend(voter)) return -1;

    return position().distance(friends_.at(voter)->position());
}

}
