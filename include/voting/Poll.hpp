#pragma once
#include <vector>
#include <unordered_map>
#include <eris/Agent.hpp>

namespace voting {

/** A polling agent for the voting model.  This class isn't really an agent that makes an
 * optimization decision, it simply conducts a new poll at the beginning of each period and stores
 * the results, providing the results for other objects (i.e. parties) to consider when deciding
 * where to move.
 */
class Poll : public eris::Agent {
    public:

        struct poll {
            std::unordered_map<eris::eris_id_t, int> closest_party;
            // FIXME: what other polling information might be useful?
        };

        /** When the period advances, a new poll is conducted and added to the beginning of the poll
         * vector, which can then be obtained by calling results().
         */
        void advance() override;

        /** Returns the poll at the current period plus n, where \f$n=0\f$ is the poll conducted at
         * the beginning of the current period.  Thus \f$n=-1\f$ is last periods poll, \f$n=-2\f$ is
         * two periods ago, etc.
         *
         * \param n how many periods relative to the current period to access in the poll history.
         * Should be 0 or negative (positive values will cause an exception).
         *
         * \throws std::out_of_range exception if n is larger than the size of the poll history.
         */
        const poll& results(int n = 0);

        /** Returns the vector of poll results within which element 0 corresponds to the first poll
         * conducted, and the last element corresponds to the current period poll.
         */
        const std::vector<poll>& allResults();

    private:
        std::vector<poll> results_;
};

}
// vim:tw=100
