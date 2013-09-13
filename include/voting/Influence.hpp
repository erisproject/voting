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
         * friends.  See also the `rand_influencees` option, which changes the interpretation of
         * this option.
         *
         * \param drift controls how far voters that have been previously influenced drift back to
         * their initial ("natural") position.  The default is 0, i.e. voters move only when being
         * influenced by other voters.  Note that drift is only applied if a voter was not
         * (successfully) influenced in the current inter-period optimization round.
         *
         * \param rand_influencees if false (the default), `probability` controls the probability
         * with which influence happens; if influence happens, it applies to all friends.  When this
         * parameter is true, the probability applies to whether the influencer attempts to
         * influence each friend.  For example, for `probability=0.1`, `rand_influencees=false`
         * means "10% of the time (probabilistically) attempt to influence all friends" while
         * `rand_influencees=true` means "Influence 10% of friends (probabilistically) each
         * period.".
         */
        Influence(eris_id_t voter, double probability, double drift = 0.0, bool rand_influencees = false);

        /** Applies influence. */
        virtual void apply();

        /** Drifts back to the voter's natural inclinations. Drift does not occur if the voter was
         * moved by influence in the current inter-period optimization round. */
        virtual void postAdvance();

    protected:
        eris_id_t voter_;
        double prob_;
        double drift_;
        bool rand_inflees_;

        /** Takes the map of the voter's friends and returns a list of `eris_id_t`s reflecting
         * friends that the voter should attempt to influence this period.  The default
         * implementation depends on the rand_influencees constructor parameter: if false, the
         * returned list is empty with probability `1-probability` and simply all the input keys
         * with probability `probability`.
         */
        virtual const std::list<eris_id_t> influencees(const std::unordered_map<eris_id_t, SharedMember<Voter>> &friends);

};

}


// vim:tw=100
