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
        // Inherited constructors is ideal here, but support is lacking, so just replicate the one
        // we used:
        //using PositionalAgent::PositionalAgent;
        Party(double p, double b1, double b2) : PositionalAgent(p, b1, b2) {}

        /** Overridden to make movements bind to the position boundaries. 
         */
        virtual bool moveToBoundary() const noexcept override { return true; }

    protected:
        /// Adds a PartyMover inter-optimizer when added
        virtual void added() override;

    private:
        // The distribution from which we pull step sizes for this Party's PartyMover
        std::uniform_real_distribution<double> step_dist_ = std::uniform_real_distribution<double>(1.0e-10, 0.1);


};

}
