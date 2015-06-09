#include "../include/Solver.h"


std::vector<int> select_random_genes(std::vector<Range> ranges, RngStream *prng)
{
    // randomly choose a gene from within each set of genes (represented by intervals)
    std::vector<int> result = std::vector<int>(ranges.size(), 0);
    int low, high;
    for (size_t index = 0; index != ranges.size(); ++ index)
    {
        low = ranges[index].low;
        high = ranges[index].high;
        result[index] = prng->RandInt(low, high-1);
    }

    return result;
}


void assign_pairwise_distance_score(Solution& sol, std::vector< std::vector<double> > distance_matrix)
{
    // assign score to solution, sum of all pairwise distances
    double score = 0.;
    std::vector<int> genes = sol.get_genes();
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
                                  std::vector< std::vector<double> > distance_matrix,
                                  std::vector<Range> ranges)
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
                          std::vector< std::vector<double> > distance_matrix,
                          std::vector<Range> ranges,
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



std::vector<Solution> initialize_population(std::vector<Range> ranges,
                                            int pop_size,
                                            RngStream *prng)
{
    // initialize the population
    assert(pop_size > 0);
    std::vector<Solution> population = std::vector<Solution>();

    // create random initial, until the population is filled
    for (int iteration = 0; iteration != pop_size; ++iteration)
    {
        population.push_back(Solution(select_random_genes(ranges, prng)));
    }
    return population;
}
