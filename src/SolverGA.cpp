#include "../include/SolverGA.h"


using std::vector;


struct int_double
{
    int best_gene;
    double cost_difference;
};


// constructors and destructors
SolverGA::SolverGA( // data
                    std::vector< std::vector<double> > distance_matrix,
                    std::vector<Range> ranges,

                    // GA settings
                    int population_size,
                    int num_generations,
                    int improvement_depth,
                    int elite_size,

                    double crossover_prob,
                    double mutation_prob,
                    double improvement_prob,

                    // misc parameters
                    std::string problem_name,
                    bool silent)
{
    
    _distance_matrix = distance_matrix;
    _ranges = ranges;
    
    _population_size = population_size;
    _elite_size = elite_size;
    _num_generations = num_generations;
    _improvement_depth = improvement_depth;
    
    _crossover_prob = crossover_prob;
    _mutation_prob = mutation_prob,
    _improvement_prob = improvement_prob;
    
    _problem_name = problem_name;
    _silent = silent;
}

SolverGA::~SolverGA() { }


void assign_pairwise_distance_score(Solution& sol, vector< vector<double> > distance_matrix)
{
    // assign score to solution, sum of all pairwise distances
    double score = 0.;
    vector<int> genes = sol.get_genes();
    int len = genes.size();
    int gene1, gene2;
    for (int i = 0; i < len; ++i)
    {
        gene1 = genes[i];
        for (int j = 0; j < len; ++j)
        {
            // add the distance
            gene2 = genes[j];
            score += distance_matrix[gene1][gene2];
        }
    }
    sol.set_score(score);
    return;
}


int_double find_best_substitution(std::vector<int> genes,
                                  int replacement_position,
                                  vector< vector<double> > distance_matrix,
                                  vector<Range> ranges)
{
    // find the best replacement for the allele at position
    double original_cost, current_cost, best_cost;
    int original_gene, best_gene;
    int gene_length = genes.size();

    // calculate the original cost (without change)
    original_cost = 0.;
    original_gene = genes[replacement_position];
    for (int i = 0; i != gene_length; ++i)
    {
        original_cost += distance_matrix[genes[i]][original_gene];
    }

    // for every possible substitution of the gene at replacement position
    // check if improvement is possible and remember the best
    best_gene = genes[replacement_position];
    best_cost = std::numeric_limits<double>::max();

    for (int current_gene = ranges[replacement_position].low;
            current_gene != ranges[replacement_position].high;
            ++current_gene)
    {

        // calculate the cost with the current replacement
        genes[replacement_position] = current_gene;
        current_cost = 0.;
        for(int i = 0; i != gene_length; ++i)
        {
            current_cost += distance_matrix[genes[i]][current_gene];
        }

        // verify if best found yet, if so, remember the new gene
        if (current_cost <= best_cost)
        {
            best_gene = current_gene;
            best_cost = current_cost;
        }
    }
    int_double ret = {best_gene, best_cost - original_cost};
    return ret;
}


void steepest_improvement(Solution& sol,
                          vector< vector<double> > distance_matrix,
                          vector<Range> ranges,
                          int max_iter)
{
    // steepest descent by finding best substitution of a gene
    assert(max_iter > 0);

    // some declarations
    int best_position, best_substitution;
    int length = sol.get_genes().size();
    double best_score;
    int_double substitution;
    int iteration = 0;

    while (iteration < max_iter)
    {
        iteration += 1;
        best_position = -1;
        best_substitution = -1;
        best_score = 0.;
        for (int replacement_position = 0; replacement_position != length; ++replacement_position)
        {
            substitution = find_best_substitution(sol.get_genes(), replacement_position, distance_matrix, ranges);
            if (substitution.cost_difference < best_score)
            {
                best_position = replacement_position;
                best_substitution = substitution.best_gene;
                best_score = substitution.cost_difference;
            }
        }
        // check if a local minima is reached
        if (best_substitution == -1)
        {
            break;
        }
        else
        {
            sol.set_gene(best_position, best_substitution);
        }
    }

    // recalculate the score
    assign_pairwise_distance_score(sol, distance_matrix);
    return;
}


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


vector<int> select_random_genes(vector<Range> ranges, RngStream *prng)
{
    // randomly choose a gene from within each set of genes (represented by intervals)
    vector<int> result = vector<int>(ranges.size(), 0);
    int low, high;
    for (size_t index = 0; index != ranges.size(); ++ index)
    {
        low = ranges[index].low;
        high = ranges[index].high;
        result[index] = prng->RandInt(low, high-1);
    }

    return result;
}


vector<Solution> initialize_population(vector<Range> ranges, int pop_size, RngStream *prng)
{
    // initialize the population
    assert(pop_size > 0);
    vector<Solution> population = vector<Solution>();

    // create random initial, until the population is filled
    for (int iteration = 0; iteration != pop_size; ++iteration)
    {
        population.push_back(Solution(select_random_genes(ranges, prng)));
    }
    return population;
}


std::vector<Solution> SolverGA::solve(unsigned long seeds[6]) const
{
    // solve the consensus problem using a genetic algorithm

    // adding an assert message by using the trick from
    //  http://stackoverflow.com/questions/3692954/add-custom-messages-in-assert
    assert(_population_size > 0 && "Illegal population size");
    assert(_num_generations > 0 && "Illegal number of generations");
    assert(_elite_size >= 0 && "Illegal elite size (below zero)");
    assert(_elite_size <= _population_size && "Illegal elite size (bigger than population");
    assert(0 <= _crossover_prob && "Illegal crossover probability (lower than zero)");
    assert(_crossover_prob <= 1 && "Illegal crossover probability (higher than one)");
    assert(0 <= _mutation_prob && "Illegal mutation probability (lower than zero)");
    assert(_mutation_prob <= 1 && "Illegal mutation probability (higher than one)");

    // seed the pseudorandom generator (MRG32k3a from L'Ecuyer)
    RngStream * prng = new RngStream();
    prng->SetSeed(seeds);

    // some declarations for later
    vector< vector<Solution> > best_solutions = vector< vector<Solution> >();
    vector<Solution> current_best_solutions;
    vector<Solution> children, parents, elite;
    Solution parent1;
    Solution parent2;
    double current_best_score;
    vector<Solution>::iterator sol_it;

    // start the progress meter
    ProgressBar bar = ProgressBar(_problem_name);

    // initialize the population
    vector<Solution> population = initialize_population(_ranges, _population_size, prng);

    // main loop
    for (size_t generation_index = 0; generation_index != _num_generations; ++generation_index)
    {

        // output the progress bar if not silent
        if (! _silent)
            bar.update( ((float) generation_index) / _num_generations);

        // score the solutions and sort the population by score
        for (sol_it = population.begin(); sol_it != population.end(); ++sol_it)
        {
            assign_pairwise_distance_score(*sol_it, _distance_matrix);
        }
        std::sort(population.begin(), population.end());

        // remember the best solutions of the current generation
        current_best_solutions = vector<Solution>();
        current_best_score = population[0].get_score(); // because it is sorted
        for (sol_it = population.begin(); sol_it != population.end(); ++sol_it)
        {
            if (sol_it->get_score() == current_best_score)
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
            if (elite.size() == _elite_size)
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
        parents = binary_tournament_selection(population, ((_population_size - elite.size())*2), prng);
        children = vector<Solution>();
        for (size_t i = 0; i != _population_size - _elite_size; ++i)
        {
            parent1 = parents[i*2];
            parent2 = parents[(i*2)+1];
            Solution child;

            // crossover
            if (prng->RandU01() < _crossover_prob)
            {
                child = uniform_crossover(parent1, parent2, 0.5, prng);
            }
            else
            {
                child = Solution(parent1);
            }

            // mutation
            if (prng->RandU01() < _mutation_prob)
                uniform_mutate(child, _ranges, prng);

            // improvement
            if (prng->RandU01() < _improvement_prob)
                steepest_improvement(child, _distance_matrix, _ranges, _improvement_depth);

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
    if (! _silent)
        bar.clean();

    // cleanup
    delete prng;

    return best_solutions.back();
}
