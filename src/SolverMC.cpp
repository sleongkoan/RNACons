#include "../include/SolverMC.h"


SolverMC::SolverMC(int sample_size,
                   bool silent) : Solver(silent)
{
    sample_size_ = sample_size;
}


SolverMC::~SolverMC() { }


std::vector<Solution> SolverMC::solve(std::vector< std::vector<double> > distance_matrix,
                                      std::vector<Range> ranges,
                                      unsigned long seeds[6]) const
{
    // solve the consensus problem using
    // Monte Carlo + steepest descent
    assert(sample_size_ > 0 && "Nonsensical sample size");

    int improvement_depth = std::numeric_limits<int>::max();

    // seed the pseudorandom generator (MRG32k3a from L'Ecuyer)
    RngStream* prng = new RngStream();
    prng->SetSeed(seeds);

    // create solutions and add them to the solutions
    // if they score better or equal to current best known
    std::vector<Solution> solutions = std::vector<Solution>();
    double current_best_score = std::numeric_limits<double>::max();

    // start the progress meter
    ProgressBar bar = ProgressBar();
    for (size_t index = 0; index != sample_size_; ++index)
    {
        // display progress
        if (! silent_)
        {
            bar.update((float) index / sample_size_);
        }
        // create a new solution
        Solution current_solution = initialize_population(ranges, 1, prng)[0];
        // improve it
        steepest_improvement(current_solution, distance_matrix, ranges, improvement_depth);
        // score it
        assign_pairwise_distance_score(current_solution, distance_matrix);
        // if better, update the best known score
        if (current_solution.get_score() <= current_best_score)
        {
            // update the best and add current to solutions
            current_best_score = current_solution.get_score();
            solutions.push_back(current_solution);
        }
    }


    // find the best unique solutions
    std::vector<Solution> best_solutions = std::vector<Solution>();
    for (size_t index = 0; index != solutions.size(); ++index)
    {
        if (solutions[index].get_score() == current_best_score)
        {
            // check if it already in the best solutions
            bool found = false;
            for (size_t i = 0; i < best_solutions.size(); ++i)
            {
                if (best_solutions[i] == solutions[index])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                // add to the best solutions
                best_solutions.push_back(solutions[index]);
            }
        }
    }
    if (! silent_)
    {
        bar.clean();
    }

    delete prng;
    return best_solutions;
}
