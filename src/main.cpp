#include <eris/Eris.hpp>
#include <eris/Simulation.hpp>
#include <eris/Position.hpp>
#include <eris/Random.hpp>
#include "voting/Voter.hpp"
#include "voting/Party.hpp"
#include "voting/PartyMover.hpp"
#include "voting/Poll.hpp"
#include "voting/Influence.hpp"
#include "voting/election/FPTP.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <vector>
#include <algorithm>
#include <limits>
#include <map>
#include <tclap/CmdLine.h>

using namespace voting;
using namespace std::chrono;

void voterHist(const Eris<Simulation> &sim) {
    const int bin_count = 100; // NB: doesn't include +1 for the ">" bin
    const double bin_start = -1;
    const double bin_end = 1;
    const double bin_width = (bin_end - bin_start) / bin_count;
    std::vector<int> bins(bin_count+1, 0);
    for (auto v: sim->agentFilter<Voter>()) {
        double p = v.second->position()[0];
        unsigned int bin = p < bin_start ? 0 : (p - bin_start) / bin_width;
        if (bin >= bins.size()) bin = bins.size()-1;
        bins[bin]++;
    }

    std::list<std::string> rows;

    for (int i = 0; i < 25; i++) {
        std::ostringstream row;
        for (auto &count : bins) {
            int eighths = count - i * 8;

            row << (eighths >= 8 ? u8"█" :
                    eighths == 7 ? u8"▇" :
                    eighths == 6 ? u8"▆" :
                    eighths == 5 ? u8"▅" :
                    eighths == 4 ? u8"▄" :
                    eighths == 3 ? u8"▃" :
                    eighths == 2 ? u8"▂" :
                    eighths == 1 ? u8"▁" : " ");
        }
        row << "\n";
        rows.push_front(row.str());
    }
    for (auto &s : rows) std::cout << s;

    for (unsigned int i = 1; i < bins.size() - 1; i++) {
        if ((i-1) % 10 == 0 and i <= bins.size() - 2) {
            printf(u8"↑=%-8.4f", bin_start + bin_width*(i-1));
        }
    }
    std::cout << ">\n";

    std::vector<int> party_bins(bin_count+1, 0);
    for (auto pp: sim->agentFilter<Party>()) {
        double p = pp.second->position()[0];
        unsigned int bin = p < bin_start ? 0 : (p - bin_start) / bin_width;
        if (bin >= party_bins.size()) bin = party_bins.size()-1;
        party_bins[bin]++;
    }
    for (auto &count : party_bins) {
        std::cout << (
                count > 4 ? std::to_string(count) :
                count == 4 ? u8"█" :
                count == 3 ? u8"▓" :
                count == 2 ? u8"▒" :
                count == 1 ? u8"░" :
                             " ");
    }
    std::cout << "\n\n";
}

enum voter_dist { dist_even, dist_uniform, dist_beta22, dist_beta55 };
enum election_type { elect_periodic, elect_random }; //, elect_normal };
struct prog_params {
    unsigned int voters = 999;
    unsigned int parties = 3;
    unsigned int iterations = 1000;
    double friend_prob = 0.01;
    double influence_prob = 0.1;
    double drift = 0.05;
    bool influence_all = false;
    bool constr_parties = false;
    bool show_hist = false;
    double constr_left = 2; // Anything > 1 is considered unset.
    voter_dist vdist = dist_beta55;
    election_type etype = elect_periodic;
    unsigned int eperiod = 50;
//    double enorm_mean = 50;
//    double enorm_stdev = 50;
};
template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
class RangeConstraint : public TCLAP::Constraint<T> {
    public:
        RangeConstraint(T min) : min_(min), no_max_(true) {}
        RangeConstraint(T min, T max) : min_(min), max_(max) {}
        virtual std::string description() const override {
            if (max_ == std::numeric_limits<double>::infinity())
                return "Value must be at least " + std::to_string(min_);
            else
                return "Value must be between " + std::to_string(min_) + " and " + std::to_string(max_);
        }
        virtual std::string shortID() const override {
            if (no_max_)
                return "v >= " + std::to_string(min_);
            else
                return std::to_string(min_) + " <= v <= " + std::to_string(max_);
        }
        virtual bool check(const T &val) const override {
            return val >= min_ and val <= max_;
        }
    private:
        T min_;
        T max_ = std::numeric_limits<T>::has_infinity ? std::numeric_limits<T>::infinity() : std::numeric_limits<T>::max();
        bool no_max_ = false;
};

prog_params parseCmdArgs(int argc, char **argv) {
    prog_params p;

    try {
        TCLAP::CmdLine cmd("Eris-based voter/party simulation model", ' ', "0.0.1");
        RangeConstraint<unsigned int> partiesConstr(1, 10);
        TCLAP::ValueArg<unsigned int> partiesArg("p", "parties", "Number of parties to create", false, p.parties,
                &partiesConstr, cmd);

        TCLAP::SwitchArg constrArg("c", "constrained", "Constrain parties to +/- 0.5 of initial location.",
                cmd, p.constr_parties);

        RangeConstraint<double> constrLeftConstr(-1, 1);
        TCLAP::ValueArg<double> constrLeftArg("L", "constrain-left-below", "The upper constraint for the left-most party.  Defaults to 1 (unconstrained) if parties are not constrained, "
                "otherwise defaults to the initial party location.",
                false, p.constr_left, &constrLeftConstr, cmd);

        RangeConstraint<unsigned int> votersConstr(1);
        TCLAP::ValueArg<unsigned int> votersArg("v", "voters", "Number of voters to create", false, p.voters,
                &votersConstr, cmd);

        std::vector<std::string> dists;
        dists.push_back("even");
        dists.push_back("uniform");
        dists.push_back("beta22");
        dists.push_back("beta55");
        TCLAP::ValuesConstraint<std::string> distValues(dists);
        TCLAP::ValueArg<std::string> distArg("d", "voter-distribution", "Distribution of voters: 'even' spreads voters evenly over the range; "
                "'uniform' uses a random uniform distribution; 'beta22' use a beta(2,2) distribution; 'beta55' uses a beta(5,5) distribution.",
                false, p.voters == dist_even ? "even" : p.voters == dist_uniform ? "uniform" : p.voters == dist_beta22 ? "beta22" : "beta55",
                &distValues, cmd);

        RangeConstraint<unsigned int> iterConstr(1);
        TCLAP::ValueArg<unsigned int> iterArg("i", "iterations", "Number of simulation iterations", false, p.iterations,
                &iterConstr, cmd);

        TCLAP::SwitchArg histArg("H", "histogram", "Show histogram of voter positions with party positions after each iteration.  Requires "
                "a terminal supporting/expecting UTF-8 output.", cmd, p.show_hist);

        RangeConstraint<double> friendConstr(0, 1);
        RangeConstraint<double> inflConstr(0, 1);
        RangeConstraint<double> driftConstr(0, 2);

        TCLAP::ValueArg<double> friendArg("F", "friend-probability", "The probability of any voter being a friend of any other voter",
                false, p.friend_prob, &friendConstr, cmd);
        TCLAP::ValueArg<double> inflArg("I", "influence-probability", "The probability that a voter will attempt to exhert influence on each friend",
                false, p.influence_prob, &inflConstr, cmd);
        TCLAP::ValueArg<double> driftArg("D", "drift-rate", "The speed at which influenced friends drift back to their initial location",
                false, p.drift, &driftConstr, cmd);

        TCLAP::SwitchArg inflAllArg("A", "influence-all", "If provided, voters attempt to exhert influence on their entire set of friends at once, with probability given by -I.  The default "
                "behaviour attempts to exhert influence on each voter with the probability given by -I.",
                cmd, p.influence_all);

        std::vector<std::string> electionTypes;
        electionTypes.push_back("periodic");
        electionTypes.push_back("random");
        //electionTypes.push_back("normal");
        TCLAP::ValuesConstraint<std::string> electionValues(electionTypes);
        TCLAP::ValueArg<std::string> electionArg("e", "election", "Election type: 'periodic' holds elections at fixed periods; "
                "'random' holds an election in each period with a random probability.",
                //; 'normal' draws each election interval from a truncated normal distribution.",
                false, p.etype == elect_periodic ? "periodic" : "random",
                &electionValues, cmd);

        RangeConstraint<unsigned int> ePeriodConstr(1);
        TCLAP::ValueArg<unsigned int> ePeriodArg("E", "election-period", "If --election=period: the period of an election.  If --election=random: "
                "the mean election period length (also the inverse election probability).", false, p.eperiod, &ePeriodConstr, cmd);

        cmd.parse(argc, argv);

        p.parties = partiesArg.getValue();
        p.constr_parties = constrArg.getValue();
        p.voters = votersArg.getValue();
        p.vdist = distArg.getValue() == "even" ? dist_even : distArg.getValue() == "uniform" ? dist_uniform :
            distArg.getValue() == "beta22" ? dist_beta22 : dist_beta55;
        p.show_hist = histArg.getValue();
        p.friend_prob = friendArg.getValue();
        p.influence_prob = inflArg.getValue();
        p.influence_all = inflAllArg.getValue();
        p.constr_left = constrLeftArg.getValue();
        p.drift = driftArg.getValue();
        p.iterations = iterArg.getValue();
        p.etype = electionArg.getValue() == "periodic" ? elect_periodic : elect_random;
        p.eperiod = ePeriodArg.getValue();

        return p;
    }
    catch (TCLAP::ArgException &e) {
        std::cerr << "Error: " << e.error() << " for arg " << e.argId() << "\n";
        throw;
    }
}

int main(int argc, char **argv) {

    auto params = parseCmdArgs(argc, argv);
    std::string run_details = std::to_string(params.parties) + " parties (" +
        (params.constr_parties ? "" : "un") + "constrained), " +
        std::to_string(params.voters) + " voters (initial dist: " +
        (params.vdist == dist_even ? "even" : params.vdist == dist_uniform ? "uniform" :
         params.vdist == dist_beta22 ? "beta(2,2)" : "beta(5,5)") +
        "); F=" + std::to_string(params.friend_prob) + "; I=" + std::to_string(params.influence_prob) +
        "; A=" + std::to_string(params.influence_all) + "; D=" + std::to_string(params.drift);

    Eris<Simulation> sim;

    // Used for output purposes: party 0 is left-most, 1 is second-left-most, etc.
    std::map<eris_id_t, int> parties;

    for (int i = 1; i <= params.parties; i++) {
        double pos = -1.0 + i*2.0 / (params.parties+1);
        double left_con = -1.0, right_con = 1.0;
        if (params.constr_parties) {
            left_con = std::max(pos - 0.5, -1.0);
            right_con = std::min(pos + 0.5, 1.0);
        }
        if (i == 1 and params.constr_left <= 1) {
            right_con = params.constr_left;
        }
        auto p = sim->createAgent<Party>(pos, left_con, right_con);

        parties[p] = i;
    }

    // Candidate num voters, based on having single element quantiles up to (only the smallest 5 listed for each)
    // 24: 5354228879
    // 22: 232792559, 465585119, 698377679, 931170239, 1163962799
    // 18: 12252239, 24504479, 36756719, 49008959, 61261199
    // 16: 720719, 1441439, 2162159, 2882879, 3603599
    // 15: 360359, 1081079, 1801799, 2522519, 2522519
    // 12: 27719, 55439, 83159, 110879, 138599
    // 10: 2519, 5039, 7559, 10079, 12599
    //  8: 839, 1679, 3359, 4199, 5879
    //  7: 419, 1259, 2099, 2939, 3779
    //  6: 59, 119, 179, 239, 299
    //  4: 11, 23, 35, 47, 71
    //  3: 5, 17, 29, 41, 53

    auto rng = Random::rng();
    std::function<double(int)> positioner;
    if (params.vdist == dist_even) {
        // Uniform initial positioner:
        positioner = [&](int i) -> double { return -1 + 2 * i / (params.voters - 1.0); };
    }
    else if (params.vdist == dist_uniform) {
        // Random uniform initial positioner:
        std::uniform_real_distribution<double> runif(-1.0, 1.0);
        positioner = [&](int i) -> double { return runif(rng); };
    }
    else {
        // Random beta(a,b) distributed (rescaled from [0,1] to [-1,1])
        double beta_a = 5.0, beta_b = 5.0;
        std::gamma_distribution<double> gamma_a(beta_a, 1), gamma_b(beta_b, 1);
        positioner = [&](int i) -> double {
            double x = gamma_a(rng);
            double y = gamma_b(rng);
            return -1 + 2 * (x / (x + y));
        };
    }


    for (int i = 0; i < params.voters; i++) {
        auto voter = sim->createAgent<Voter>(positioner(i), -1, 1);
        sim->createInterOpt<Influence>(voter, params.influence_prob, params.drift, params.influence_all);
    }

    std::bernoulli_distribution friend_flip(params.friend_prob);
    // Create some friendships
    for (auto &v1 : sim->agentFilter<Voter>()) {
        for (auto &v2 : sim->agentFilter<Voter>()) {
            if (v1.first != v2.first and friend_flip(rng))
                v1.second->addFriend(v2.first);
        }
    }

    auto pollster = sim->createAgent<Poll>();

    SharedMember<FPTP> election = (params.etype == elect_periodic)
        ? sim->createAgent<FPTP>(params.eperiod)
        : sim->createAgent<FPTP>([&params,&rng]() -> bool { return std::bernoulli_distribution(1.0 / params.eperiod)(rng); });

    std::ostringstream filename;
    filename << "results/elections:p" << params.parties << (params.constr_parties ? ",c" : ",!c");
    if (params.constr_left > 1) filename << ",!L"; else filename << ",L=" << params.constr_left;
    filename << ",v" << params.voters;
    filename << ",d" << (params.voters == dist_even ? "even" : params.voters == dist_uniform ? "uniform" : params.voters == dist_beta22 ? "beta22" : "beta55");
    filename << ",i" << params.iterations << ",F" << params.friend_prob << ",I" << params.influence_prob << ",D" << params.drift;
    filename << (params.influence_all ? ",A" : ",!A") << ",e" << (params.etype == elect_periodic ? "periodic" : "random");
    filename << ",E" << params.eperiod << ",seed=" << eris::Random::seed() << ".data";

    std::ofstream outfile;
    outfile.open(filename.str(), std::ofstream::out | std::ofstream::trunc);
    if (!outfile) {
        std::perror(("Unable to write to " + filename.str() + ": ").c_str());
        exit(1);
    }
    outfile.precision(18);
    outfile << "t,winner,position\n";

    std::cout << "\e[?25l"; // Hide cursor (from `tput civis`)
    std::cout << "\e[2J"; // clear screen
    std::cout << "\e[2;0H"; // Position at line 1, col 0

    if (params.show_hist) voterHist(sim);

    std::cout << "\e[1;0H"; // Position at line 0, col 0
    std::cout << "Running (" << run_details << ")\n";
    std::cout << "Results data: " << filename.str() << "\n";

    std::list<double> winner_pos, winnerNL_pos;
    double winner_mean = std::numeric_limits<double>::quiet_NaN();
    double winner_sd = std::numeric_limits<double>::quiet_NaN();
    double winnerNL_mean = std::numeric_limits<double>::quiet_NaN();
    double winnerNL_sd = std::numeric_limits<double>::quiet_NaN();

    auto last = steady_clock::now();
    for (int i = 1; i <= params.iterations; i++) {
        sim->run();
        auto now = steady_clock::now();
        double speed = i == 1
            ? std::numeric_limits<double>::quiet_NaN()
            : 1.0/duration_cast<duration<double>>(now - last).count();
        last = now;
        printf("\e[3;0HIteration %d (%8.2f iterations/s)\n", i, speed);

        if (params.show_hist)
            voterHist(sim);

        if (election->electionPeriod()) {
            auto winner = sim->agent<Party>(election->election());
            auto pos = winner->position().at(0);
            winner_pos.push_back(pos);

            winner_mean = std::accumulate(winner_pos.cbegin(), winner_pos.cend(), 0.0) / winner_pos.size();
            winner_sd = sqrt(std::accumulate(winner_pos.cbegin(), winner_pos.cend(), 0.0, [&](const double sd, const double x) {
                return sd + (x - winner_mean) * (x - winner_mean);
            }) / (winner_pos.size() - 1));

            if (parties[winner] != 1) { // Not the left-most party
                winnerNL_pos.push_back(pos);
                winnerNL_mean = std::accumulate(winnerNL_pos.cbegin(), winnerNL_pos.cend(), 0.0) / winnerNL_pos.size();
                winnerNL_sd = sqrt(std::accumulate(winnerNL_pos.cbegin(), winnerNL_pos.cend(), 0.0, [&](const double sd, const double x) {
                    return sd + (x - winnerNL_mean) * (x - winnerNL_mean);
                }) / (winnerNL_pos.size() - 1));
            }

            outfile << i << "," << parties[winner] << "," << pos << "\n";
        }

        printf("Winning party positions: last: %8.6f, mean: %8.6f, sd: %8.6f\n",
                winner_pos.empty() ? winner_mean : winner_pos.back(), winner_mean, winner_sd);

        printf("Non-left winning party positions: last: %8.6f, mean: %8.6f, sd: %8.6f\n",
                winnerNL_pos.empty() ? winnerNL_mean : winnerNL_pos.back(), winnerNL_mean, winnerNL_sd);

    }

    std::cout << "\e[?12;25h"; // Restore cursor (from `tput cvvis`)

    outfile.close();

}
