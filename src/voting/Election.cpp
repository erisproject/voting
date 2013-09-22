#include "voting/Election.hpp"

namespace voting {

Election::Election(unsigned int period) {
    election_ = [period,this]() -> bool {
        if (++election_counter_ >= period) {
            election_counter_ = 0;
            return true;
        }
        return false;
    };
}

Election::Election(std::function<bool()> callElection) : election_(callElection) {}

bool Election::electionPeriod() const {
    return election_period_;
}

void Election::advance() {
    election_period_ = election_();
}

}
