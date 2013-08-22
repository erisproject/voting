#pragma once
#include <vector>
#include <unordered_map>
#include <eris/Agent.hpp>
#include <eris/Position.hpp>

namespace voting {

using eris::eris_id_t;
using eris::Position;

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

        /** Conducts a poll at the current party positions and returns it. */
        virtual poll conductPoll();

        /** Conducts a poll at the current party positions except for party `party_id`, which will
         * be considered to be at position `try_pos` rather than its current position.
         */
        virtual poll conductPollIf(eris::eris_id_t party_id, Position try_pos);
};

}
// vim:tw=100
