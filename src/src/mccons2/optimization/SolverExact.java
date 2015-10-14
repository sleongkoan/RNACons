package mccons2.optimization;

import mccons2.util.Pair;
import mccons2.util.WidthComparator;

import java.util.ArrayList;
import java.util.Collections;
import java.util.PriorityQueue;



class SubSolution extends Solution{


    private ArrayList<Pair<Integer, Integer>> ranges;


    public SubSolution(ArrayList<Integer> genes_,
                       double score_,
                       ArrayList<Pair<Integer, Integer>> ranges_)
    {
        super(genes_, score_);
        ranges = ranges_;
    }


    public ArrayList<Pair<Integer, Integer>> getRanges() {
        return ranges;
    }


    public String toString()
    {
        // add the genes
        StringBuilder builder = new StringBuilder();
        builder.append("genes ");
        for (Integer i : getGenes())
        {
            builder.append(i + " ");
        }
        builder.append(System.lineSeparator());

        // add the score
        builder.append("score " + getScore() + System.lineSeparator());

        // add the remaining ranges
        builder.append("ranges" + System.lineSeparator());
        for( Pair<Integer, Integer> r : getRanges())
        {
            builder.append("[ " + r.getFirst() + " .. " + r.getSecond() + " ]");
            builder.append(System.lineSeparator());
        }
        return builder.toString();
    }


    public Solution toSolution()
    {
        return new Solution(getGenes(), getScore());
    }
}




public class SolverExact extends Solver {


    private double threshold;
    private boolean verbose;

    public boolean isVerbose()
    {
        return verbose;
    }

    public SolverExact(double suboptimal_threshold, boolean verbose_)
    {
        threshold = suboptimal_threshold;
        verbose = verbose_;
    }


    /**
     * find closest elements in other range and the get
     * @param self_index
     * @param range
     * @param distanceMatrix
     * @return
     */
    public static double findSmallestDistanceRange(int self_index, Pair<Integer, Integer> range,
                                                   double[][] distanceMatrix) {

        double best_distance = Double.POSITIVE_INFINITY;
        double distance;

        for (int i = range.getFirst(); i != range.getSecond(); ++i) {
            distance = distanceMatrix[self_index][i];
            if (distance < best_distance) {
                best_distance = distance;
            }
        }
        return best_distance;
    }


    /**
     * using the get matrix, find closest neighbors for
     * each object, in each other groups (range)
     * @param ranges
     * @param distanceMatrix
     * @return
     */
    double[][] nearestNeighborDistanceElemToRange(ArrayList<Pair<Integer, Integer>> ranges,
                                                  double[][] distanceMatrix) {

        // matrix is (distanceMatrix.size() x ranges.size())
        double[][] elem_range_dm = new double[distanceMatrix.length][ranges.size()];

        // for each elem, for each range, find the closest get between the two
        for (int elem_index = 0; elem_index != distanceMatrix.length; ++elem_index) {
            for (int range_index = 0; range_index != ranges.size(); ++range_index) {
                elem_range_dm[elem_index][range_index] =
                        findSmallestDistanceRange(elem_index, ranges.get(range_index), distanceMatrix);
            }
        }
        return elem_range_dm;
    }


    boolean isCompleteSolution(SubSolution sol) {
        // checks whether or not a solution is complete
        return (sol.getRanges().size() == 0);
    }


    void branch(SubSolution solution,
                double[][] elem_range_dm,
                double[][] distanceMatrix,
                PriorityQueue<SubSolution> search_space,
                double cutoff) {
        // used to create all children of a node
        // and add them, if they are promising enough, to the
        // search space (which is a max heap)
        assert (isCompleteSolution(solution) == false);

        // separate the ranges, the one to explore and the rest
        Pair<Integer, Integer> range_to_explore = solution.getRanges().get(0);
        ArrayList<Pair<Integer, Integer>> new_ranges = new ArrayList<>();

        for (int i = 1; i != solution.getRanges().size(); ++i) {
            new_ranges.add(solution.getRanges().get(i));
        }

        for (int new_gene = range_to_explore.getFirst();
             new_gene != range_to_explore.getSecond();
             ++new_gene) {

            // create the new genes
            ArrayList<Integer> new_genes = new ArrayList<>();
            for (int g = 0; g != solution.getGenes().size(); ++g) {
                new_genes.add(solution.getGenes().get(g));
            }
            new_genes.add(new_gene);

            // create the new solution, score it and add it if its worth it
            SubSolution new_solution = new SubSolution(new_genes, Double.POSITIVE_INFINITY, new_ranges);
            assignPairwiseDistanceScore(new_solution, distanceMatrix);

            if (new_solution.getScore() <= cutoff) {
                search_space.add(new_solution);
            }
        }
    }


    public ArrayList<Solution> solve(double[][] distanceMatrix, ArrayList<Pair<Integer, Integer>> ranges)
    {
        // use a priority queue to represent the current search space
        PriorityQueue<SubSolution> searchSpace = new PriorityQueue<>();
        ArrayList<SubSolution > satisfyingSolutions = new ArrayList<>();
        double bestScore = Double.POSITIVE_INFINITY;
        double leeway = threshold * (ranges.size() * (ranges.size() - 1));
        double scoreThreshold = bestScore + leeway;

        // sort the ranges by their size (to avoid big branching at the start)
        ArrayList<Pair<Integer, Integer> > sortedRanges = new ArrayList<>(ranges);
        Collections.sort(sortedRanges, new WidthComparator());


        // initialize the search space with the empty subsolution
        searchSpace.add(new SubSolution(new ArrayList<Integer>(), Double.POSITIVE_INFINITY, sortedRanges));
        double[][] elemToRangeDistanceMatrix = nearestNeighborDistanceElemToRange(sortedRanges, distanceMatrix);


        // Branch & Bound main loop
        while (searchSpace.size() > 0) {
            scoreThreshold = bestScore + leeway;
            SubSolution currentSolution = searchSpace.poll();

            if (isCompleteSolution(currentSolution)) {
                // check if worth adding to satisfying solutions
                if ((currentSolution.getScore() <= scoreThreshold)) {
                    satisfyingSolutions.add(currentSolution);
                }
                // check if the score was actually the best seen and update if it is
                if (currentSolution.getScore() < bestScore) {
                    bestScore = currentSolution.getScore();
                }
            } else {  // not a complete solution
                branch(currentSolution, elemToRangeDistanceMatrix, distanceMatrix, searchSpace, scoreThreshold);
            }
        }

        // filter out solutions by score knowing now the best possible score
        ArrayList<Solution > bestSolutions = new ArrayList<>();
        for (int i = 0; i != satisfyingSolutions.size(); ++i) {
            // add only those satisfying scoring constraints
            if (satisfyingSolutions.get(i).getScore() <= scoreThreshold) {
                bestSolutions.add(satisfyingSolutions.get(i).toSolution());
            }
        }
        return bestSolutions;
    }
}