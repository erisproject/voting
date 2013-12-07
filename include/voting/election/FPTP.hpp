#pragma once
#include <eris/Random.hpp>
#include "voting/Election.hpp"
#include "voting/Party.hpp"
#include "voting/Voter.hpp"
#include <unordered_map>
#include <stdexcept>

namespace voting {

/** First-past-the-post election mechanism.  The party with the largest number of votes wins.  If
 * two or more parties are tied for the highest number of votes, the winner is determined randomly.
 * Each voter votes for the nearest party, randomizing to resolve ties.
 */
class FPTP : public Election, public virtual eris::interopt::Advance {
    public:
        //using Election::Election;

		/** Creates an election that occurs every `period` simulation periods.
		 */
		FPTP(unsigned int period) : Election(period) {}

		/** Creates an election that occurs every time the given function/lambda returns true.  The
		 * function will be called exactly once per inter-optimization, and, when it returns true,
		 * will cause the following period to be an election round.
		 */
		FPTP(std::function<bool()> callElection) : Election(callElection) {}

        /** Conducts and election (if one hasn't been performed already) and returns the winner.
         */
        virtual eris_id_t election() override;

        /** Resets the winner and vote tallies when advancing.
         */
        virtual void interAdvance() override;

        /** Returns the map of parties to votes each party received in the election.  Performs the
         * election first, if it hasn't yet been performed.
         */
        virtual const std::unordered_map<eris_id_t, int>& votes();

    private:
        eris_id_t winner_ = 0;
        std::unordered_map<eris_id_t, int> votes_;
};

}

// vim:tw=100
