#pragma once
#include <eris/Positional.hpp>
#include <eris/Agent.hpp>
#include <eris/Optimize.hpp>
#include <unordered_map>
#include <random>

namespace voting {

using eris::eris_id_t;
using eris::Positional;
using eris::Agent;

/** A "Party" eris agent.  A party has a Position and moves around.
 */
class Party : public Positional<Agent>, public virtual eris::interopt::OptApply  {
    public:
        // Inherited constructors is ideal here, but support is lacking, so just replicate the one
        // we used:
        //using Positional<Agent>::Positional<Agent>;
        Party(double p, double b1, double b2) : Positional<Agent>({p}, {b1}, {b2}) {}

        /** Overridden to make movements bind to the position boundaries. 
         */
        virtual bool moveToBoundary() const noexcept override { return true; }

        /** Figures out whether the party should move left (down) or right (up) for the next
         * iteration by calling should_move().
         */
        virtual void interOptimize() override;

        /** Figures out whether the party should increase or decrease for the next iteration, and
         * returns the movement.  This happens by conducting a poll for one step upward and one step
         * downward and a poll for the current position, choosing whichever direction yields the
         * best response (or not moving at all if that seems best).  In the case of a tie (between
         * two or all three options) an option is chosen randomly.
         *
         * If the party is at a boundary it only considers moving away or staying.
         *
         * Returns the amount to move by.
         */
        virtual double should_move();

        /** Takes whatever step was calculated in optimize(). */
        virtual void interApply() override;

    private:
        // The distribution from which we pull step sizes for this Party's PartyMover
        std::uniform_real_distribution<double> step_dist_{1.0e-10, 0.1};

        double move_;

};

}
