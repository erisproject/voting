#pragma once
#include <eris/algorithms.hpp>
#include <eris/InterOptimizer.hpp>
#include <functional>

namespace voting {

using eris::InterOptimizer;
using eris::eris_id_t;

class PartyMover : public InterOptimizer {
    public:
        /** Constructs a party movement optimizer that takes a constant step size each iteration. */
        PartyMover(eris_id_t party, double step_size);
        /** Constructs a party movement optimizer that calls step_generator to obtain a new step
         * size for each iteration. The step generator should always return a strictly positive
         * value. */
        PartyMover(eris_id_t party, std::function<double()> step_generator);

    protected:
        /** Figures out whether the party should move left (down) or right (up) for the next
         * iteration by calling should_move().
         */
        virtual void optimize() const override;

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
        virtual double should_move() const;

        /** Takes whatever step was calculated in optimize(). */
        virtual void apply() override;

        /** Declares a dependency on the controlled party when added. */
        virtual void added() override;

    private:
        eris_id_t party_;

        std::function<double()> step_gen_;

        mutable double move_;
};

}

// vim:tw=100
