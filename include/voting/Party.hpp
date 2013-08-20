#pragma once
#include <eris/Position.hpp>
#include <eris/agent/PositionalAgent.hpp>
#include <unordered_map>

namespace voting {

using eris::eris_id_t;
using eris::agent::PositionalAgent;

/** A "Party" eris agent.  A party has a Position and moves around.
 */
class Party : public PositionalAgent {
    public:
        using PositionalAgent::PositionalAgent;

        /** The ID if the pollster.  If this is not set when needed, it'll be updated to the first
         * Poll agent returned from the simulation.  If you want to do anything with multipler
         * pollsters, you'll have to make sure this gets set to the right one rather than rely on
         * the automatic updating.
         */
        eris_id_t pollster = 0;


};

}
