package optimization;

import distances.DistanceFunction;
import distances.StringEditDistance;
import distances.TreeEditDistance;
import rna.GranularBasePairTree;
import util.Readers;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.Hashtable;


public class MCCons {


    static class shapeAndSkeletonDist implements DistanceFunction<String, String, Double> {
        public int shapeLevel;
        public double mult1, mult2, mult3;

        shapeAndSkeletonDist(double mult1_, double mult2_) {
            //assert(shapeLevel_ == 1 || shapeLevel_ == 3 || shapeLevel_ ==5);
            //shapeLevel = shapeLevel_;
            mult1 = mult1_;
            mult2 = mult2_;
            //mult3 = mult3_;
        }

        public Double get(String dotBracket1, String dotBracket2) {
            /*
            OrderedRootedLabeledTree completeTree1 = new OrderedRootedLabeledTree(dotBracket1, '(', ')', '.');
            OrderedRootedLabeledTree completeTree2 = new OrderedRootedLabeledTree(dotBracket2, '(', ')', '.');

            String shape1 = RNAshapes.dotBracketToAbstractShape(dotBracket1, shapeLevel);
            String shape2 = RNAshapes.dotBracketToAbstractShape(dotBracket2, shapeLevel);

            OrderedRootedLabeledTree shapeTree1 = new OrderedRootedLabeledTree(shape1, '[', ']', '_');
            OrderedRootedLabeledTree shapeTree2 = new OrderedRootedLabeledTree(shape2, '[', ']', '_');
*/

            TreeEditDistance dist = new TreeEditDistance(1, 1, 1);
            StringEditDistance stringDist = new StringEditDistance();

            // first distance
            // Double wholeDistance = dist.get(completeTree1, completeTree2);
            Double skeletonDistance = dist.get(dotBracket1, dotBracket2);
            //Double shapeDistance = dist.get(shapeTree1, shapeTree2);
            Double stringDistance = stringDist.get(dotBracket1, dotBracket2);

            return (mult1 * skeletonDistance) + (mult2 * stringDistance);
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
                              optimization.Solver firstSolver,
                              Solver secondSolver) throws IOException {

        // PHASE 1: TREE CONSENSUS


        // data acquisition
        ArrayList<ArrayList<String>> dotBrackets = Readers.readMarnaFile(path);
        Hashtable<String, String> reverseMapping = new Hashtable<>();

        ArrayList<ArrayList<String>> converted = new ArrayList<>();

        for (ArrayList<String> list : dotBrackets) {
            // initialize a vector of trees to add later
            HashSet<String> uniqueStrings = new HashSet<>();
            for (String dotBracket : list) {
                // check if it has already been processed
                String conv = new GranularBasePairTree(dotBracket, 3).toString();
                uniqueStrings.add(conv);
                //System.out.println(dotBracket + " = " + conv);
            }
            // add to the converted objects
            ArrayList<String> uniques = new ArrayList<>();
            for (String string : uniqueStrings) {
                uniques.add(string);
            }
            converted.add(uniques);
        }


        shapeAndSkeletonDist compoundDist = new shapeAndSkeletonDist(1., 0.);
        ConsensusProblem<String> treeProblem = new ConsensusProblem<>(converted, compoundDist);
        int num_comparisons = dotBrackets.size() * (dotBrackets.size() - 1);  // n (n -1)


        // PHASE 1 SOLVING
        boolean verbose = firstSolver.isVerbose();
        if (verbose) {
            System.err.println("Phase 1: Base Pair Tree Consensus (1)");
        }

        ArrayList<Solution> treeConsensus = firstSolver.solve(treeProblem.getDistanceMatrix(), treeProblem.getRanges());


        ArrayList<ArrayList<String>> uniqueSolutions = new ArrayList<>();
        int i = 0;
        for (Solution solution : treeConsensus) {
            ArrayList<String> consensus = treeProblem.getObjects(solution.getGenes());
            Collections.sort(consensus);
            if (!uniqueSolutions.contains(consensus)) {
                uniqueSolutions.add(consensus);

                System.out.println("> solution " + i + " : " + solution.getScore());
                for (String dotBracket : consensus) {
                    System.out.println(dotBracket);
                }
                System.out.println();
                i += 1;
            }
        }
    }
}


        // sort the tree consensus by score (lower is better)

/*
        System.out.println("phase 1.5");

        // PHASE 1.5: FILTERING
        ArrayList<ArrayList<ArrayList<String>>> filteredData = new ArrayList<>();

        // get the brackets
        for (Solution solution : treeConsensus) {
            ArrayList<String> brackets = new ArrayList<>();
            for (Integer gene : solution.getGenes()) {
                brackets.add(treeProblem.getObjects().get(gene).toString());
            }

            // filter and add to the problem 2 data
            ArrayList<ArrayList<String>> filtered_brackets = filter_dot_brackets(dotBrackets, brackets);
            filteredData.add(filtered_brackets);
        }
        ArrayList<ConsensusProblem<String>> dot_bracket_problems = new ArrayList<>();

        // instantiate the problems
        for (int i = 0; i != filteredData.size(); ++i) {
            dot_bracket_problems.add(new ConsensusProblem<>(filteredData.get(i), new StringEditDistance()));
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
        for (int i = 0; i != filteredData.size(); ++i) {
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