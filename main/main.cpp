/*
 * Author: Tingsheng (Tinson) Lai 781319
 */

// @formatter:off

#include <cstdio>

#include <iostream>

#ifdef MULTI_NODE
#include <boost/mpi.hpp>
#endif
#include <boost/timer/timer.hpp>

#include "include/grid.h"

#if defined(SINGLE_THREAD)
#include "include/processors/single_thread_processor.h"
#elif defined(MULTI_NODE)
#include "include/processors/multi_node_processor.h"
#else
#include "include/processors/multi_thread_processor.h"
#endif

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Minimum of 1 parameter is expected, %d was provided", argc - 1);
        return -1;
    }

    #ifdef MULTI_NODE
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    #endif

    boost::timer::cpu_timer timer;
    timer.start();
    auto g = argc == 3 ? grid(argv[2]) : grid();
    timer.stop();
    std::cout << timer.format(3, "Grid Reading:\nWall: %w\nUser: %u\nSystem: %s\nTotal: %t\nPercentage: %p%\n\n")
              << std::endl;

    #if defined(SINGLE_THREAD)
    single_thread_processor p(argv[1], g);
    #elif defined(MULTI_NODE)
    multi_node_processor p(env, world, argv[1], g);
    #else
    multi_thread_processor p(argv[1], g);
    #endif
    timer.start();
    p.preprocess();
    timer.stop();
    std::cout << timer.format(3, "Preprocessing:\nWall: %w\nUser: %u\nSystem: %s\nTotal: %t\nPercentage: %p%\n\n")
              << std::endl;

    timer.start();
    auto output = p();
    timer.stop();
    std::cout << timer.format(3, "Final Processing:\nWall: %w\nUser: %u\nSystem: %s\nTotal: %t\nPercentage: %p%\n\n")
              << std::endl;

    for (auto const & [k, c, _] : output)
        printf("%s: %lu\n", k.c_str(), c);
    for (auto const & [k, _, htp] : output)
    {
        printf("%s: (", k.c_str());
        auto it = htp.begin();
        for (; it != htp.end() - 1; ++it)
        {
            auto & [t, c] = *it;
            printf("(#%s, %lu)", t.c_str(), c);
        }
        auto & [t, c] = *it;
        printf("(#%s, %lu))\n", t.c_str(), c);
    }

    return 0;
}
