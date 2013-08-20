#pragma once
#include <eris/algorithms.hpp>
#include <eris/interopt/InterStepper.hpp>

namespace voting {

using eris::interopt::InterStepper;
using eris::eris_id_t;

class PartyMover : public eris::interopt::InterStepper {
    public:
        static constexpr double default_min_step = 1.0/1048576.0;
        PartyMover(
                eris_id_t party,
                double initial_step = InterStepper::default_initial_step,
                double increase_count = InterStepper::default_increase_count,
                double min_step = default_min_step);

    protected:
        /** Returns true if we should increase next period, which will happen in four cases:
         * 
         * - the last step was an increase and polls increased
         * - the last step was a decrease and polls decreased
         * - the current position is at the party's lower bound
         * - there is no poll history, and the party is positioned >= 0.
         *
         * In all other cases, this method returns false.
         *
         * If one of the bounds is the "best" position, we'll end up alternating between up and down
         * movements, which will trigger Stepper to crank down the step size, making the party stay
         * very close to the bound.
         *
         * The last case just defines the default movement to be "away from 0", which treats
         * positive and negative parties symmetrically.  It isn't perfectly symmetric if there is a
         * party exactly at zero, of course, so this also picks "up" as the arbitrary direction.
         */
        virtual bool should_increase() const override;

        /** Takes the given step.  Keeps track of constraints; a step that would move outside of the
         * party's position constraints is truncated to the violated constraint.
         */
        virtual void take_step(double step) override;

        /** Declares a dependency on the controlled party when added. */
        virtual void added() override;

    private:
        eris_id_t party_;
};

}

// vim:tw=100
