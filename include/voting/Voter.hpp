#pragma once
#include <eris/Position.hpp>
#include <eris/agent/PositionalAgent.hpp>
#include <eris/Random.hpp>
#include <unordered_map>

namespace voting {

using namespace eris;
using eris::agent::PositionalAgent;

/** A "Voter" eris agent.  A voter has a Position and a set of friends.
 */
class Voter : public PositionalAgent {
    public:
        // Inherited constructors is ideal here, but support is lacking, so just replicate the one
        // we used:
        //using PositionalAgent::PositionalAgent;
        Voter(double p, double b1, double b2) : PositionalAgent(p, b1, b2) {}

        /** Returns true if the given voter is a friend of this voter. */
        bool isFriend(eris_id_t voter) const;
        /** Adds the given voter to this voter's friends. */
        void addFriend(eris_id_t voter);
        /** Removes the given voter from this voter's friends. */
        void removeFriend(eris_id_t voter);

        /** Provides read-only access to the unordered_map of friends of this voter. */
        const std::unordered_map<eris_id_t, SharedMember<Voter>>& friends() const;

        /** Returns the distance between the given voter and this voter.  Returns -1.0 if the given
         * voter is not a friend of this voter.
         * \sa Position::distance
         */
        virtual double friendDistance(eris_id_t voter) const;

        /** Called when the passed-in voter is attempting to influence this voter.  Returns true if
         * the influence succeeds, false if it failed.  If the influence succeeds, this voter moves
         * to the position of the influencer and sets its just_moved field to true.
         *
         * \param by the voter attempt to exhert influence
         *
         * \param drift if successful, the speed at which the influenced voter will drift back to
         * their original posiiton.
         *
         * This method calls influenceProbability to determine the probability of success.
         */
        virtual bool attemptInfluence(const SharedMember<Voter> &by, double drift);

        /** Returns the probability with which influence should succeed.
         *
         * The default implementation succeeds with probability \f$\frac{1+b-a}{2}\f$, where b is
         * the conviction of the influencer and a is the conviction of the current object (the voter
         * potentially being influenced).  Thus equal-conviction voters have a 50% chance of
         * successfully influencing each other, while maximum conviction voters are (just) always
         * successful at influencing minimum conviction voters, while minimum conviction voters have
         * (just) no chance at all of convincing maximum conviction voters.
         *
         * Note that conviction values outside [0,1] are truncated to the nearest endpoint of that
         * range.
         *
         * \param my_conv the conviction of the voter being influenced, which will be truncated to
         * [0,1] if outside that range.
         *
         * \param their_conv the conviction of the influencer, which will be truncated to [0,1] if
         * outside that range.
         */
        virtual double influenceProbability(double my_conv, double their_conv) const;

        /** Voter conviction is a function of the distance to the parties, where being closer to the
         * nearer party results in higher conviction.  This default implementation simply returns 1
         * minus the distance to the nearest party, truncated to 0 if negative.
         *
         * Note that this [0,1] return is not required, but relied upon in attemptInfluence
         * (truncating conviction values to [0,1]), so any overriding of this function that changes
         * the conviction range should also override attemptInfluence().
         */
        virtual double conviction() const;

        /** Returns the natural position of the voter, which can differ from the current position if
         * the voter has been influenced (and has not yet drifted back).
         */
        const Position naturalPosition() const;

        /** Returns true if the voter has been moved by influence in the current inter-period
         * optimization; this will be true only during the inter-period optimization apply() stage,
         * and is reset to false during the voter's inter-period advance stage.
         */
        bool justMoved() const;

        /** Sets the drift speed that the agent will drift back to their natural position.  This is
         * typically called by an Influence optimizer, which, upon successful influence, both moves
         * the voter and sets the drift.  (Thus different voters could have different staying power
         * of their influence).
         *
         * \sa voting::Influence
         */
        virtual void setDrift(double drift);

        /** Returns the voter's current drift speed.  Returns 0 if the voter is already at their
         * natural position.
         */
        virtual double drift() const;

        /** If not at the agent's natural position (i.e. the agent was moved by influence sometime
         * in the past), and drift is positive, the agent takes a step of length `drift` (or less,
         * if already within distance `drift`) towards their natural position.
         */
        virtual void advance() override;

    protected:
        /** The map of friends of this voter. */
        std::unordered_map<eris_id_t, SharedMember<Voter>> friends_;

    private:
        std::unique_ptr<const eris::Position> natural_pos_;
        bool just_moved_ = false;
        double drift_ = 0.0;
        eris::Random::rng_t &rng_ = eris::Random::rng();

};

}

// vim:tw=100
