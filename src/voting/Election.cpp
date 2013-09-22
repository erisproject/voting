#include "voting/Election.hpp"

namespace voting {

Election::Election(int period) {
    long counter;
    election_ = [period,&counter]() -> bool {
        if (++counter >= period) {
            counter = 0;
            return true;
        }
        return false;
    };
}

Election::Election(std::function<bool()> callElection) : election_(callElection) {}

}
