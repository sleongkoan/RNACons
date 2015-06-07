#include "../include/SolverMC.h"


using std::vector;
using std::string;


SolverMC::SolverMC(vector< vector<double> > distance_matrix,
                   vector<Range> ranges,
                   int sample_size,
                   string problem_name,
                   bool silent)
{
    _distance_matrix = distance_matrix;
    _ranges = ranges;

    _sample_size = sample_size;

    _problem_name = problem_name;
    _silent = silent;
}

SolverMC::~SolverMC() { }


vector<Solution> SolverMC::solve(unsigned long seeds[6])
{
    // solve the consensus problem using
    // Monte Carlo + steepest descent
    assert(_sample_size > 0 && "Nonsensical sample size");

    size_t improvement_depth = pow(10, 5);

    // seed the pseudorandom generator (MRG32k3a from L'Ecuyer)
    RngStream* prng = new RngStream();
    prng->SetSeed(seeds);

    // create solutions and score them first
    // O(n) memory, not clever I know...
    vector<Solution> solutions = initialize_population(_ranges, _sample_size, prng);
    for (size_t index = 0; index != solutions.size(); ++index)
    {
        assign_pairwise_distance_score(solutions[index], _distance_matrix);
    }

    // start the progress meter
    ProgressBar bar = ProgressBar(_problem_name);
    double best_score = std::numeric_limits<double>::infinity();


    // main loop
    for (size_t index = 0; index != _sample_size; ++index)
    {
        // display progress
        if (! _silent)
        {
            bar.update((float) index / _sample_size);
        }

        // apply optimization
        steepest_improvement(solutions[index], _distance_matrix, _ranges, improvement_depth);
        assign_pairwise_distance_score(solutions[index], _distance_matrix);
        if (solutions[index].get_score() < best_score)
        {
            best_score = solutions[index].get_score();
        }
    }

    // find the best unique solutions
    vector<Solution> best_solutions = vector<Solution>();
    for (size_t index = 0; index != _sample_size; ++index)
    {
        if (solutions[index].get_score() == best_score)
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
    if (! _silent)
    {
        bar.clean();
    }

    return best_solutions;
}
