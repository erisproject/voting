#pragma once
#include <eris/InterOptimizer.hpp>
#include "voting/Voter.hpp"
#include <list>

namespace voting {

using eris::InterOptimizer;
using eris::eris_id_t;

/** This class handles the inter-period influence adjustments of voters on other voters.
 */
class Influence : public InterOptimizer {
    public:
        /** Creates a new Influence inter-period optimizer attached to the given voter.  After each
         * period, with probability `probability`, the voter will become an activist.  An activist
         * attempts to convert his friends to adopt his current political position, with success
         * depending on the two voters' political party proximities.  If successful, the friend
         * moves to the influencer's position.
         *
         * \param voter the eris_id_t of the voter ("influencer") this Influence optimizer applies to
         *
         * \param probability the probability that a voter will attempt to exert influence on his
         * friends.  See also the `influence_all` option, which changes the interpretation of this
         * option.
         *
         * \param drift controls how far influenced voters will drift back to their initial
         * ("natural") position.  Note that this drift applies to the influenced voters, not the
         * influencer: you can think of drift as a sort of "staying power" of this influencer.  The
         * default is 0, i.e. voters move only when being influenced by other voters.  Note that
         * drift is only applied if a voter was not (successfully) influenced in the current
         * inter-period optimization round.
         *
         * \param influence_all if true (the default), `probability` controls the probability with
         * which influence happens; when influence happens, it applies to all friends.  When this
         * parameter is false, the probability applies to whether the influencer attempts to
         * influence each friend.  For example, for `probability=0.1`, `influence_all=true` means
         * "10% of the time (probabilistically) attempt to influence all friends" while
         * `influence_all=false` means "Influence 10% of friends (probabilistically) each period.".
         */
        Influence(eris_id_t voter, double probability, double drift = 0.0, bool influence_all = true);

        /** Applies influence. */
        virtual void apply();

    protected:
        eris_id_t voter_;
        double prob_;
        double drift_;
        bool infl_all_;

        /** Takes the map of the voter's friends and returns a list of `eris_id_t`s reflecting
         * friends that the voter should attempt to influence this period.  The default
         * implementation depends on the influence_all constructor parameter: if true, the
         * returned list is empty with probability `1-probability` and simply all the input keys
         * with probability `probability`; when false, each friend is added to the return list with
         * probability `probability`.
         */
        virtual const std::unordered_map<eris_id_t, SharedMember<Voter>> influencees(const std::unordered_map<eris_id_t, SharedMember<Voter>> &friends);

};

}


// vim:tw=100
