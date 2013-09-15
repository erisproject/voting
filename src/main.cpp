#include <eris/Eris.hpp>
#include <eris/Simulation.hpp>
#include <eris/Position.hpp>
#include <eris/Random.hpp>
#include "voting/Voter.hpp"
#include "voting/Party.hpp"
#include "voting/PartyMover.hpp"
#include "voting/Poll.hpp"
#include "voting/Influence.hpp"
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <chrono>

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

            row << (eighths >= 8 ? "█" :
                    eighths == 7 ? "▇" :
                    eighths == 6 ? "▆" :
                    eighths == 5 ? "▅" :
                    eighths == 4 ? "▄" :
                    eighths == 3 ? "▃" :
                    eighths == 2 ? "▂" :
                    eighths == 1 ? "▁" : " ");
        }
        row << "\n";
        rows.push_front(row.str());
    }
    for (auto &s : rows) std::cout << s;

    for (unsigned int i = 1; i < bins.size() - 1; i++) {
        if ((i-1) % 10 == 0 and i <= bins.size() - 2) {
            printf("↑=%-8.4f", bin_start + bin_width*(i-1));
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
                count >= 3 ? "█" :
                count == 2 ? "▒" :
                count == 1 ? "░" :
                             " ");
    }
    std::cout << "\n\n";
}

int main() {
    Eris<Simulation> sim;

    sim->createAgent<Party>(-0.5, -1, 1);
    sim->createAgent<Party>(0.5, -1, 1);
/*    auto p_left  = sim->createAgent<Party>(-0.5, -1, 0);
    auto p_right = sim->createAgent<Party>(0.5, 0, 1);
    auto p_centre = sim->createAgent<Party>(0, 0.5, -0.5);*/

/*    sim->createInterOpt<PartyMover>(p_left,   1.0/32, 4, 1.0/1048576, 3, 1);
    sim->createInterOpt<PartyMover>(p_centre, 1.0/32, 4, 1.0/1048576, 3, 2);
    sim->createInterOpt<PartyMover>(p_right,  1.0/32, 4, 1.0/1048576, 3, 0);*/

    //auto rng = eris::Random::rng();
    //std::uniform_real_distribution<double> runif(-1, 1);

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

    int num_voters = 199;
    //int num_voters = 2519;
    //int num_voters = 5;

    // The probability of any random voter v2 being a friend of v1.
    double friend_prob = 0.1;
    double influence_prob = 0.1;
    double drift = 0.05;
    // True: influence everyone or no one with prob (influence_prob); false:
    // influence each friend with prob (influence_prob).
    bool influence_all = false;

    // Uniform initial positioner:
    auto positioner_unif = [](int i, int num_voters) -> double { return -1 + 2 * i / (num_voters - 1.0); };

    // Random uniform initial positioner:
    std::uniform_real_distribution<double> runif(-1.0, 1.0);
    auto positioner_runif = [&runif](int i, int num_voters) -> double { return runif(Random::rng()); };

    // Random beta(a,b) distributed (rescaled from [0,1] to [-1,1])
    double beta_a = 5.0, beta_b = 5.0;
    std::gamma_distribution<double> gamma_a(beta_a, 1), gamma_b(beta_b, 1);
    auto positioner_rbeta = [&gamma_a, &gamma_b](int i, int num_voters) -> double {
        double x = gamma_a(Random::rng());
        double y = gamma_b(Random::rng());
        return -1 + 2 * (x / (x + y));
    };


    std::function<double(int, int)> positioner;
    // Which positioner to use (done this way to avoid unused variable warnings/errors)
    if (1) positioner = positioner_unif;
    else if (0) positioner = positioner_runif;
    else positioner = positioner_rbeta;

    for (int i = 0; i < num_voters; i++) {
        auto voter = sim->createAgent<Voter>(positioner(i, num_voters), -1, 1);
        sim->createInterOpt<Influence>(voter, influence_prob, drift, influence_all);
//        sim->createAgent<Voter>(runif(rng), -1, 1);
    }

    std::bernoulli_distribution friend_flip(friend_prob);
    // Create some friendships
    for (auto &v1 : sim->agentFilter<Voter>()) {
        for (auto &v2 : sim->agentFilter<Voter>()) {
            if (v1.first != v2.first and friend_flip(Random::rng()))
                v1.second->addFriend(v2.first);
        }
    }

//
//    auto v1 = sim->createAgent<Voter>(Position({-0.5}));
//    auto v2 = sim->createAgent<Voter>(Position({0.4}));
//    auto v3 = sim->createAgent<Voter>(Position({0.0}));

    auto pollster = sim->createAgent<Poll>();

/*    std::cout << "Parties:\n    id   @ position\n    ----   ---------\n";
    for (auto &p : sim->agentFilter<Party>()) {
        printf("    %-4ld @ %9.6f\n", p.first, p.second->position()[0]);
    }*/

    std::cout << "\e[?25l"; // Hide cursor (from `tput civis`)
    std::cout << "\e[2J"; // clear screen
    std::cout << "\e[2;0H"; // Position at line 1, col 0
    voterHist(sim);

    std::cout << "\e[1;0H"; // Position at line 0, col 0
    std::cout << "Running!\n";
//    if (0)
    auto last = steady_clock::now();
    for (int i = 1; i <= 1000; i++) {
        sim->run();
        auto now = steady_clock::now();
        double speed = 1.0/duration_cast<duration<double>>(now - last).count();
        last = now;
        std::cout << "\e[2;0HIteration " << i << " (" << speed << " iterations/s; " << Voter::DEBUG_influence_attempts << " influence attempts):\n";
        Voter::DEBUG_influence_attempts = 0;
/*        std::cout << "Parties: ";
        auto poll = pollster->conductPoll();
        for (auto &p : { p_left, p_centre, p_right }) {
            printf("%ld@%11.8f [%7.4f%%], ", p->id(), p->position()[0], poll.closest_party[p] * 100.0 / num_voters);
        }
        std::cout << "\n";*/

        voterHist(sim);
    }

    std::cout << "\e[?12;25h"; // Restore cursor (from `tput cvvis`)

    std::cout << "Parties:\n    id   @ position\n    ----   ---------\n";
    for (auto &p : sim->agentFilter<Party>()) {
        printf("    %-4ld @ %9.6f\n", p.first, p.second->position()[0]);
    }
    /*std::cout << "Voters:\n";
    int cols = 6, col = 0;
    std::cout << "    ";
    for (int i = 0; i < cols; i++)
        std::cout << "id  @ position    ";
    std::cout << "\n    ";
    for (int i = 0; i < cols; i++)
        std::cout << "---   ---------   ";
    for (auto &v : sim->agentFilter<Voter>()) {
        if (col++ % cols == 0)
            std::cout << "\n    ";
        printf("%-3ld @ %9.6f   ", v.first, v.second->position()[0]);
//        std::cout << "voter[" << v.first << "] at: " << v.second->position() << "\n";
    }
    std::cout << "\n";*/

    /*
    double prev_cutoff = -1.0;
    double cutoff = -1.0;
    auto cumulative = [&](SharedMember<Voter> v) { return v->position()[0] <= cutoff; };
    auto increment = [&](SharedMember<Voter> v) {
        double p = v->position()[0];
        return p <= cutoff and p > prev_cutoff;
    };
    int boxes = 20;
    for (int i = 0; i <= boxes; i++) {
        cutoff = i * 2.0 / boxes - 1;
        prev_cutoff = (i-1) * 2.0 / boxes - 1;
        int cum_count = sim->agentFilter<Voter>(cumulative).size();
        int inc_count = sim->agentFilter<Voter>(increment).size();
        std::cout << "Voters in (" << prev_cutoff << "," << cutoff << "]: " << inc_count << "; <= " << cutoff << ": " << cum_count << "\n";
    }
    */
//    std::cout << "v1 (id=" << v1->id() << ") at: " << v1->position() << "\n";
//    std::cout << "v2 (id=" << v2->id() << ") at: " << v2->position() << "\n";
//    std::cout << "v3 (id=" << v3->id() << ") at: " << v3->position() << "\n";

//    std::cout << "Adding 1\n";
//    v3->addFriend(v1);
//    std::cout << "Added 1\n";
//    v3->addFriend(v2);
//    std::cout << "Added 2\n";
//    eris_id_t v3id = v3;
//
//    std::cout << "v3id = " << v3id << "\n";
//
//    for (auto &f: v3->friends()) {
//        std::cout << "v3 friend at position " << f.second->position() << "\n";
//    }

}
