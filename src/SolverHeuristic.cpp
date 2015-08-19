#include "../include/SolverHeuristic.h"


using std::vector;


// constructors and destructors
SolverHeuristic::SolverHeuristic(// misc parameters
                   bool silent,

                   // GA settings
                   int population_size,
                   int num_generations,
                   int improvement_depth,
                   int elite_size,
                   double distance_threshold,

                   double crossover_prob,
                   double mutation_prob,
                   double improvement_prob,
                   unsigned long seeds[6]) : Solver(silent)
{
    assert(distance_threshold >= 0);
    population_size_ = population_size;
    elite_size_ = elite_size;
    distance_threshold_ = distance_threshold;
    num_generations_ = num_generations;
    improvement_depth_ = improvement_depth;

    crossover_prob_ = crossover_prob;
    mutation_prob_ = mutation_prob;
    improvement_prob_ = improvement_prob;
    seeds_ = seeds;

}

SolverHeuristic::~SolverHeuristic() { }


Solution uniform_crossover(Solution parent1,
                           Solution parent2,
                           double crossover_probability,
                           RngStream* prng)
{
    // uniform crossover operator over the genes of two solutions
    vector<int> inherited_genes = vector<int>();
    double prob; // Uniform(0, 1)
    for (size_t index = 0; index != parent1.get_genes().size(); ++index)
    {
        prob = prng->RandU01();
        if (prob < crossover_probability)
        {
            inherited_genes.push_back(parent1.get_genes()[index]);
        }
        else
        {
            inherited_genes.push_back(parent2.get_genes()[index]);
        }
    }
    return Solution(inherited_genes);
}


Solution uniform_mutate(Solution& sol,
                         std::vector<Range> ranges,
                         RngStream *prng,
                         double mutation_probability=0.1)
{
    // mutate the solution by simply swapping with a probability
    double prob;
    vector<int> mutated_genes = vector<int>(sol.get_genes().size(), 0);

    for (size_t index = 0; index != sol.get_genes().size(); ++index)
    {
        prob = prng->RandU01();
        if (prob < mutation_probability)
        {
            // exchange for a random gene within the same range
            mutated_genes[index] = prng->RandInt(ranges[index].low,
                                                 ranges[index].high-1);
        }
        else
        {
            // keep the same gene
            mutated_genes[index] = sol.get_genes()[index];
        }
    }
    return Solution(mutated_genes);
}


Range select_2(int low, int high, RngStream *prng)
{
    // select 2 different random integers in the interval
    assert(high - 1 > low);
    int first = prng->RandInt(low, high-1);
    int second = prng->RandInt(low, high-1);

    while (first == second)
    {
        second = prng->RandInt(low, high-1);
    }
    Range ret = {first, second};
    return ret;
}


vector<Solution> binary_tournament_selection(vector<Solution> population,
                                             int num_to_select,
                                             RngStream *prng)
{
    // classical binary tournament selection
    assert(num_to_select > 0);
    vector<Solution> selected = vector<Solution>();
    Range indices;
    int pop_size = population.size() - 1;
    for (int index = 0; index != num_to_select; ++index)
    {
        indices = select_2(0, pop_size, prng);
        if (population[indices.low].get_score() < population[indices.high].get_score())
        {
            selected.push_back(Solution(population[indices.low]));
        }
        else
        {
            selected.push_back(Solution(population[indices.high]));
        }
    }
    return selected;
}



std::vector<Solution> SolverHeuristic::solve(std::vector< std::vector<double> > distance_matrix,
                                             std::vector<Range> ranges) const
{
    // solve the consensus problem using a genetic algorithm

    // adding an assert message by using the trick from
    //  http://stackoverflow.com/questions/3692954/add-custom-messages-in-assert
    assert(population_size_ > 0 && "Illegal population size");
    assert(num_generations_ > 0 && "Illegal number of generations");
    assert(elite_size_ >= 0 && "Illegal elite size (below zero)");
    assert(elite_size_ <= population_size_ && "Illegal elite size (bigger than population");
    assert(0 <= crossover_prob_ && "Illegal crossover probability (lower than zero)");
    assert(crossover_prob_ <= 1 && "Illegal crossover probability (higher than one)");
    assert(0 <= mutation_prob_ && "Illegal mutation probability (lower than zero)");
    assert(mutation_prob_ <= 1 && "Illegal mutation probability (higher than one)");

    // seed the pseudorandom generator (MRG32k3a from L'Ecuyer)
    RngStream * prng = new RngStream();
    prng->SetSeed(seeds_);

    // some declarations for later
    vector< vector<Solution> > best_solutions = vector< vector<Solution> >();
    vector<Solution> current_best_solutions;
    vector<Solution> children, parents, elite;
    Solution parent1;
    Solution parent2;
    double current_best_score;
    double scaled_threshold = distance_threshold_ * ranges.size() * (ranges.size()-1);
    vector<Solution>::iterator sol_it;

    // start the progress meter
    ProgressBar bar = ProgressBar();

    // initialize the population
    vector<Solution> population = initialize_population(ranges, population_size_, prng);

    // main loop
    for (size_t generation_index = 0; generation_index != num_generations_; ++generation_index)
    {

        // output the progress bar if not silent
        if (! silent_)
            bar.update( ((float) generation_index) / num_generations_);

        // score the solutions and sort the population by score
        for (sol_it = population.begin(); sol_it != population.end(); ++sol_it)
        {
            assign_pairwise_distance_score(*sol_it, distance_matrix);
        }
        std::sort(population.begin(), population.end());

        // remember the best solutions of the current generation
        current_best_solutions = vector<Solution>();
        current_best_score = population[0].get_score(); // because it is sorted
        for (sol_it = population.begin(); sol_it != population.end(); ++sol_it)
        {
            if (sol_it->get_score() <= current_best_score + scaled_threshold)
            {
                // try to find if it exists already in the elite vector
                bool found = false;
                for (size_t j = 0; j != current_best_solutions.size(); ++j)
                {
                    if (current_best_solutions[j] == *sol_it)
                    {
                        found = true;
                        break;
                    }
                }
                if (! found)
                {
                    current_best_solutions.push_back(Solution(*sol_it));
                }
            }
            else
            {
                break;
            }
        }

        best_solutions.push_back(current_best_solutions);

        // elitist selection with only unique individuals, no repetition
        elite = vector<Solution>();

        for (size_t i = 0; i < population.size(); ++i)
        {
            if (elite.size() == elite_size_)
            {
                break;
            }
            else
            {
                bool found = false;
                for(size_t j = 0; j < elite.size(); ++j)
                {
                    if ( (population[i] == elite[j]) )
                    {
                        found = true;
                        break;
                    }
                }
                if (! found)
                {
                    elite.push_back(Solution(population[i]));
                }
            }
        }

        // selection process
        parents = binary_tournament_selection(population, ((population_size_ - elite.size())*2), prng);
        children = vector<Solution>();
        for (size_t i = 0; i != population_size_ - elite_size_; ++i)
        {
            parent1 = parents[i*2];
            parent2 = parents[(i*2)+1];
            Solution child;

            // crossover
            if (prng->RandU01() < crossover_prob_)
            {
                child = uniform_crossover(parent1, parent2, 0.5, prng);
            }
            else
            {
                child = Solution(parent1);
            }

            // mutation
            if (prng->RandU01() < mutation_prob_)
                uniform_mutate(child, ranges, prng);

            // improvement
            if (prng->RandU01() < improvement_prob_)
                steepest_improvement(child, distance_matrix, ranges, improvement_depth_);

            // children is done
            children.push_back(child);
        }

        // replace the population by its children and the previous elite
        for (sol_it = elite.begin(); sol_it != elite.end(); ++sol_it)
        {
            children.push_back(Solution(*sol_it));
        }

        // swap the two populations
        population = children;
    }

    // clean the progress bar
    if (! silent_)
        bar.clean();

    // cleanup
    delete prng;

    // keep all the unique best solutions up to a specified suboptimal threshold
    std::vector<Solution> suitable_solutions = std::vector<Solution>();
    double score_threshold = current_best_score + scaled_threshold;
    for (size_t i = 0; i != best_solutions.size(); ++i)
    {
        for(size_t j = 0; j != best_solutions[i].size(); ++j)
        {
            if (best_solutions[i][j].get_score() <= score_threshold)
            {
                // verify if the solution is already in the optimal ones
                bool found = false;
                for(size_t k = 0; k < suitable_solutions.size(); ++k)
                {
                    if ( (best_solutions[i][j] == suitable_solutions[k]) )
                    {
                        found = true;
                        break;
                    }
                }
                if (! found)
                {
                    suitable_solutions.push_back(Solution(best_solutions[i][j]));
                }
            }
        }
    }
    return suitable_solutions;
}
