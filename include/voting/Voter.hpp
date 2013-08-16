#pragma once
#include "voting/Position.hpp"
#include <eris/Agent.hpp>
#include <unordered_map>

namespace voting {

using namespace eris;

/** A "Voter" eris agent.  A voter has a Position and a set of friends.
 */
class Voter : public Agent {
    public:
        /** Constructs a voter at the given Position. */
        Voter(const Position &p);
        /** Constructs a voter at a position created at the given coordinates. */
        Voter(std::initializer_list<double> pos);
        Voter() = delete;

        virtual ~Voter() = default;

        /** Returns true if the given voter is a friend of this voter. */
        bool isFriend(eris_id_t voter);
        /** Adds the given voter to this voter's friends. */
        void addFriend(eris_id_t voter);
        /** Removes the given voter from this voter's friends. */
        void removeFriend(eris_id_t voter);

        /** Provides read-only access to the unordered_map of friends of this voter. */
        const std::unordered_map<eris_id_t, SharedMember<Voter>>& friends();

        /** Returns the distance between the given voter and this voter.  Returns -1.0 if the given
         * voter is not a friend of this voter.
         * \sa Position::distance
         */
        double friendDistance(eris_id_t voter);

        /** The geometric position of the voter. */
        Position position;

    protected:
        /** The map of friends of this voter. */
        std::unordered_map<eris_id_t, SharedMember<Voter>> friends_;

};

}
