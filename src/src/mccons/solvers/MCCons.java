package mccons.solvers;

import mccons.repr.ConsensusProblem;
import mccons.repr.OrderedRootedLabeledTree;
import mccons.repr.cost.CostFunction;
import mccons.repr.cost.DistanceFunction;
import mccons.rna.RNAshapes;
import mccons.util.Readers;
import mccons.repr.cost.TreeEditDistance;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;

public class MCCons {


     static class UnitCost implements CostFunction<Character, Double> {

        public Double get(Character elem) {
            return 1.;
        }
    }


    static class SubstitutionCost implements DistanceFunction<Character, Character, Double> {
        public Double get(Character a, Character b) {
            Double dist;
            if (a.compareTo(b) == 0) {
                dist = 0.;
            } else {
                dist = 2.;
            }

            return dist;
        }
    }


    /**
     * given a solver and a problem instance, apply the MC-Cons algorithm
     *
     * @param path         path of the input file
     * @param firstSolver  solver for the tree problems
     * @param secondSolver solver for the dot bracket problems
     */
    public static void MCCONS(String path,
                              Solver firstSolver,
                              Solver secondSolver) throws IOException {

        // PHASE 1: TREE CONSENSUS

        // get functions
        UnitCost indel = new UnitCost();
        SubstitutionCost sub = new SubstitutionCost();
        TreeEditDistance treeDist = new TreeEditDistance(indel, indel, sub);

        // data acquisition
        ArrayList<ArrayList<String>> dotBrackets = Readers.readMarnaFile(path);
        ArrayList<ArrayList<OrderedRootedLabeledTree>> trees = new ArrayList<>();

        for (ArrayList<String> list : dotBrackets) {
            // initialize a vector of trees to add later
            HashSet<String> uniqueStrings = new HashSet<>();
            ArrayList<OrderedRootedLabeledTree> uniqueTrees = new ArrayList<>();

            for (String string : list) {
                // check if it has already been processed
                String shape = RNAshapes.dotBracketToAbstractShape(string, 3);
                if (!uniqueStrings.contains(shape)) {
                    uniqueStrings.add(shape);
                    uniqueTrees.add(new OrderedRootedLabeledTree(shape, '[', ']', '_'));
                }
            }
            // add to the trees
            trees.add(uniqueTrees);
        }

        ConsensusProblem<OrderedRootedLabeledTree> treeProblem = new ConsensusProblem<>(trees, treeDist);
        int num_comparisons = dotBrackets.size() * (dotBrackets.size() - 1);  // n (n -1)


        // PHASE 1 SOLVING
        boolean verbose = firstSolver.isVerbose();
        if (verbose) {
            System.err.println("Phase 1: Base Pair Tree Consensus (1)");
        }

        ArrayList<Solution> treeConsensus = firstSolver.solve(treeProblem.getDistanceMatrix(),
                treeProblem.getRanges());

        for (Solution solution : treeConsensus) {
            System.err.println("consensus with score " + solution.getScore());
            ArrayList<OrderedRootedLabeledTree> consensus = treeProblem.getObjects(solution.getGenes());

            for (OrderedRootedLabeledTree t : consensus) {
                System.err.println(t);
            }
        }
    }
}
        // sort the tree consensus by score (lower is better)

/*
        System.out.println("phase 1.5");

        // PHASE 1.5: FILTERING
        ArrayList<ArrayList<ArrayList<String>>> prob2_data = new ArrayList<>();

        // get the brackets
        for (Solution solution : treeConsensus) {
            ArrayList<String> brackets = new ArrayList<>();
            for (Integer gene : solution.getGenes()) {
                brackets.add(treeProblem.getObjects().get(gene).toString());
            }

            // filter and add to the problem 2 data
            ArrayList<ArrayList<String>> filtered_brackets = filter_dot_brackets(dotBrackets, brackets);
            prob2_data.add(filtered_brackets);
        }
        ArrayList<ConsensusProblem<String>> dot_bracket_problems = new ArrayList<>();

        // instantiate the problems
        for (int i = 0; i != prob2_data.size(); ++i) {
            dot_bracket_problems.add(new ConsensusProblem<>(prob2_data.get(i), new StringEditDistance()));
        }


        System.out.println("phase 2");
        // PHASE 2: STRING CONSENSUS
        if (verbose) {
            // update on the number of optimal trees found
            System.err.println(System.lineSeparator() + "Phase 2: String Edit Distance on Vienna Dot Bracket (" +
                    dot_bracket_problems.size() + ")");
        }
        // if too many output, make the solver silent and add a progress
        // bar over all tree consensus instead of individually
        if ((treeConsensus.size() > 10)) {
            verbose = false;
        }
        ArrayList<ArrayList<Solution>> dotBracketConsensus = new ArrayList<>();
        ProgressBar progress = new ProgressBar("", 40);
        for (int i = 0; i != prob2_data.size(); ++i) {
            dotBracketConsensus.add(secondSolver.solve(dot_bracket_problems.get(i).getDistanceMatrix(),
                    dot_bracket_problems.get(i).getRanges()));
            progress.update((float) i / treeConsensus.size());
        }
        progress.clean();

        if (verbose) {
            System.err.println();
        }

        // printing the results to stdout
        // print the best dotbracket consensus for each tree consensus
        double tree_score, best_dot_bracket_score;
        int sol_index = 0;
        int treeProblemIndex = 0;
        for (ArrayList<Solution> solutions : dotBracketConsensus) {
            // figure out what the best scores are within each tree consensus
            tree_score = treeConsensus.get(treeProblemIndex).getScore() / num_comparisons;
            best_dot_bracket_score = Double.POSITIVE_INFINITY;

            for (Solution solution : solutions) {
                if (solution.getScore() < best_dot_bracket_score) {
                    best_dot_bracket_score = solution.getScore();
                }
            }

            for (Solution solution : solutions) {
                if (solution.getScore() == best_dot_bracket_score) {
                    // output it
                    double normalizedScore = best_dot_bracket_score / num_comparisons;
                    StringBuilder builder = new StringBuilder();
                    builder.append("> ");
                    builder.append(sol_index);       // index
                    builder.append(" ");
                    builder.append(tree_score);      // first score
                    builder.append(" ");
                    builder.append(normalizedScore); // normalized second score
                    builder.append(System.lineSeparator());
                    for (Integer gene : solution.getGenes()) {
                        String object = dot_bracket_problems.get(treeProblemIndex).getObjects().get(gene);
                        builder.append(object);
                        builder.append(System.lineSeparator());
                    }
                    System.out.println(builder.toString());
                    sol_index += 1;
                }
            }
            treeProblemIndex += 1;
        }
    }
}
*/