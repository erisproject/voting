#include <eris/Eris.hpp>
#include <eris/Simulation.hpp>
#include <eris/Position.hpp>
#include <eris/Random.hpp>
#include "voting/Voter.hpp"
#include "voting/Party.hpp"
#include <iostream>
#include <cstdio>

using namespace voting;

int main() {
    Eris<Simulation> sim;

    auto p_left  = sim->createAgent<Party>(-0.5, -1, 0);
    auto p_right = sim->createAgent<Party>(0.5, 0, 1);
    auto p_centre = sim->createAgent<Party>(0, 0.5, -0.5);
    auto rng = eris::Random::rng();
    if (false) rng = rng;
    std::uniform_real_distribution<double> runif(-1, 1);

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

    int num_voters = 2519;
    //int num_voters = 5;

    for (int i = 0; i < num_voters; i++) {
        sim->createAgent<Voter>(-1 + 2 * i / (num_voters - 1.0), -1, 1);
//        sim->createAgent<Voter>(runif(rng), -1, 1);
    }
//
//    auto v1 = sim->createAgent<Voter>(Position({-0.5}));
//    auto v2 = sim->createAgent<Voter>(Position({0.4}));
//    auto v3 = sim->createAgent<Voter>(Position({0.0}));

    std::cout << "Parties:\n    id   @ position\n    ----   ---------\n";
    for (auto &p : sim->agentFilter<Party>()) {
        printf("    %-4ld @ %9.6f\n", p.first, p.second->position()[0]);
    }
    std::cout << "Voters:\n";
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
    std::cout << "\n";

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
