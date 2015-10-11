package mccons.optimization;

import java.util.*;

import mccons.util.Pair;
import mccons.util.ProgressBar;
import mccons.util.RngStream;
import mccons.util.Util;


class ReverseSolutionComp implements Comparator<Solution>
{

    @Override
    public int compare(Solution first, Solution second) {
        return -1 * first.compareTo(second);
    }
}


public class SolverHeuristic extends Solver {

    // Generic solver parameters parameters
    private boolean verbose;
    private double tolerance;
    private long[] seeds;

    // Genetic algorithm settings
    private int populationSize;
    private int numGenerations;
    private double eliteRatio;

    private double crossoverProbability;
    private double crossoverMixingRatio;

    private double mutationProbability;
    private double mutationStrength;

    // Local search settings
    private double improvementProbability;
    private int improvementDepth;

    public SolverHeuristic(boolean verbose, double tolerance, long[] seeds,
                           int populationSize, int numGenerations, double eliteRatio,
                           double crossoverProbability, double crossoverMixingRatio,
                           double mutationProbability, double mutationStrength,
                           double improvementProbability, int improvementDepth) {
        this.verbose = verbose;
        this.tolerance = tolerance;
        this.seeds = seeds;
        this.populationSize = populationSize;
        this.numGenerations = numGenerations;
        this.eliteRatio = eliteRatio;
        this.crossoverProbability = crossoverProbability;
        this.crossoverMixingRatio = crossoverMixingRatio;
        this.mutationProbability = mutationProbability;
        this.mutationStrength = mutationStrength;
        this.improvementProbability = improvementProbability;
        this.improvementDepth = improvementDepth;
    }


    @Override
    public boolean isVerbose() {
        return verbose;
    }

    /**
     * uniform crossover operator over the genes of two solutions
     *
     * @param parent1 first parent solution
     * @param parent2 second parent solution
     * @param stream  pseudo-random generator stream
     * @return a crossover between both parents :P
     */
    public static Solution uniformCrossover(Solution parent1, Solution parent2, double mixingRatio, RngStream stream) {

        ArrayList<Integer> newGenes = parent1.getGenes();
        int size = parent1.getGenes().size();

        for (int index = 0; index != size; ++index) {
            if (stream.randU01() < mixingRatio) {
                newGenes.set(index, parent2.getGenes().get(index));
            }
        }
        return new Solution(newGenes, Double.POSITIVE_INFINITY);
    }


    /**
     * @param solution solution to mutate
     * @param ranges (begin, end) indices of each sets of objects
     * @param mutationProbability probability for the mutation of a gene
     * @param stream pseud-random number generator
     * @return mutated solution
     */
    public Solution uniformMutate(Solution solution,
                                  ArrayList<Pair<Integer, Integer>> ranges,
                                  double mutationProbability,
                                  RngStream stream) {
        // mutate the solution by simply swapping with a probability
        ArrayList<Integer> mutated_genes = solution.getGenes();
        int gene_size = mutated_genes.size();
        for (int index = 0; index != gene_size; ++index) {
            if (stream.randU01() < mutationProbability) {
                // exchange for a random gene within the same range
                mutated_genes.set(index, stream.randInt(ranges.get(index).getFirst(), ranges.get(index).getSecond() - 1));
            }
        }
        return new Solution(mutated_genes, Double.POSITIVE_INFINITY);
    }


    Pair<Integer, Integer> select2(int low, int high, RngStream stream) {
        // select 2 different random integers in the interval
        assert (high - 1 > low);
        int first = stream.randInt(low, high - 1);
        int second = stream.randInt(low, high - 1);

        while (first == second) {
            second = stream.randInt(low, high - 1);
        }
        return new Pair<>(first, second);
    }


    /**
     * select parents for the next generation, using a binary tournament selection
     *
     * @param population population over which the selection is applied
     * @param numToSelect number of individuals to select from the tournament
     * @param stream pseudo-random number generator
     * @return list of solutions selected as parents for the next generation
     */
    ArrayList<Solution> binaryTournamentSelection(ArrayList<Solution> population,
                                                  int numToSelect,
                                                  RngStream stream) {
        // classical binary tournament selection
        assert (numToSelect > 0);
        Pair<Integer, Integer> indices;
        ArrayList<Solution> selected = new ArrayList<>();
        int popSize = population.size() - 1;

        for (int index = 0; index != numToSelect; ++index) {
            indices = select2(0, popSize, stream);
            Solution first = population.get(indices.getFirst());
            Solution second = population.get(indices.getSecond());

            if (first.compareTo(second) < 0) {
                selected.add(new Solution(first));
            } else {
                selected.add(new Solution(second));
            }
        }
        return selected;
    }


    /**
     * solve the consensus problem
     * using an hybrid strategy (genetic algorithm + steepest descent)
     *
     * @param distanceMatrix matrix of distances between all objects
     * @param ranges (begin, end) indices to tell sets of objects apart
     * @return list of solutions to the consensus problem
     */
    public ArrayList<Solution> solve(double[][] distanceMatrix,
                                     ArrayList<Pair<Integer, Integer>> ranges) {
        assert (populationSize > 0);
        assert (numGenerations > 0);
        assert (eliteRatio >= 0 && eliteRatio <= 1.0);
        assert (0. <= crossoverProbability && crossoverProbability <= 1.);
        assert (0. <= crossoverMixingRatio && crossoverMixingRatio <= 1.);
        assert (0. <= mutationProbability && mutationProbability <= 1.);

        // seed the pseudo-random generator
        RngStream stream = new RngStream();
        stream.setSeed(seeds);

        // some declarations for later
        PriorityQueue hallOfFame = new PriorityQueue<>(populationSize, new ReverseSolutionComp());
        int eliteSize = (int) Math.floor(eliteRatio * populationSize);

        // start the progress meter
        ProgressBar bar = new ProgressBar("", 40);

        // initialize the population
        ArrayList<Solution> population = initializeRandomSolutions(ranges, populationSize, stream);

        // main loop
        for (int generation_index = 0; generation_index != numGenerations; ++generation_index) {

            // output the progress bar if not silent
            if (verbose) {
                bar.update(((float) generation_index) / numGenerations);
            }

            // score the solutions and sort the population by score
            for (Solution solution : population) {
                assignPairwiseDistanceScore(solution, distanceMatrix);
            }
            Collections.sort(population);
            assert Util.isSorted(population);


            // update the hall of fame
            for (Solution solution : population)
            {
                if(! hallOfFame.contains(solution))
                {
                    hallOfFame.add(new Solution(solution));
                }
                if (hallOfFame.size() > populationSize)
                {
                    hallOfFame.poll();
                }
            }


            // elitist selection with only unique individuals, no repetition
            ArrayList<Solution> elite = new ArrayList<>();
            for (Solution solution : population) {
                if (elite.size() >= eliteSize) {
                    break;
                } else if (!elite.contains(solution)) {
                    elite.add(new Solution(solution));
                }
            }

            // selection process
            ArrayList<Solution> parents = binaryTournamentSelection(population, ((populationSize - elite.size()) * 2), stream);
            ArrayList<Solution> children = new ArrayList<>();
            for (int i = 0; i != populationSize - eliteSize; ++i) {
                Solution parent1 = parents.get(i * 2);
                Solution parent2 = parents.get((i * 2) + 1);
                Solution child;

                // crossover
                if (stream.randU01() < crossoverProbability) {
                    child = uniformCrossover(parent1, parent2, crossoverMixingRatio, stream);
                } else {
                    child = new Solution(parent1);
                }

                // mutation
                if (stream.randU01() < mutationProbability) {
                    uniformMutate(child, ranges, mutationStrength, stream);
                }

                // improvement
                if (stream.randU01() < improvementProbability)
                    steepestDescent(child, distanceMatrix, ranges, improvementDepth);

                // children is complete
                children.add(child);
            }

            // replace the population by its children and the previous elite
            for (Solution solution : elite) {
                children.add(new Solution(solution));
            }

            // swap the two populations
            population = children;
        }

        // clean the progress bar
        if (verbose)
            bar.clean();


        // keep all the unique best solutions up to a specified suboptimal threshold
        ArrayList<Solution> suitableSolutions = new ArrayList<>();
        double scaledThreshold = tolerance * ranges.size() * (ranges.size() - 1);
        ArrayList<Solution> hallOfFameList = new ArrayList();
        while (hallOfFame.size() > 0)
        {
            hallOfFameList.add((Solution) hallOfFame.poll());
        }
        Collections.sort(hallOfFameList);
        double scoreThreshold = hallOfFameList.get(0).getScore() + scaledThreshold;
        for (Solution solution : hallOfFameList) {
            if ((solution.getScore() <= scoreThreshold) && (!suitableSolutions.contains(solution))) {
                    suitableSolutions.add(new Solution(solution));
                }
            }
        return suitableSolutions;
    }
}

