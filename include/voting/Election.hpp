#pragma once
#include <functional>
#include <eris/Agent.hpp>
#include <eris/Optimize.hpp>

namespace voting {

using eris::eris_id_t;

/** A election agent for the voting model.  This class isn't really an agent that makes an
 * optimization decision, it simply sets an election state every x periods.  In this special
 * election state, regular polling and party movement stops and an election takes place, with this
 * class determining the winner.
 *
 * This is an abstract class in that it doesn't actually implement calculation of the winner;
 * subclasses must provide the election() method to do that.
 */
class Election : public eris::Agent, public virtual eris::interopt::Advance {
    public:
		/** Creates an election that occurs every `period` simulation periods.
		 */
		Election(unsigned int period);

		/** Creates an election that occurs every time the given function/lambda returns true.  The
		 * function will be called exactly once per inter-optimization, and, when it returns true,
		 * will cause the following period to be an election round.
		 */
		Election(std::function<bool()> callElection);

        /** Returns true if the current period is an election period.
         */
        bool electionPeriod() const;

		/** Conducts an election at the current positions, returns the winner.  Abstract method.
		 */
		virtual eris_id_t election() = 0;
        
        /** Advances, checking whether an election should be called.  If so, electionPeriod() will
         * return true until the following advance.
         */
        virtual void interAdvance() override;

	private:
        bool election_period_ = false;
        unsigned int election_counter_ = 0;
        std::function<bool()> election_;
};

}
// vim:tw=100

