package optimization;

import distances.DistanceFunction;
import distances.StringEditDistance;
import distances.TreeEditDistance;
import rna.GranularBasePairTree;
import rna.GranularRepr;
import rna.RNAConverter;
import util.ProgressBar;
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
                              Solver firstSolver,
                              Solver secondSolver) throws IOException {

        // #################### PHASE 1: COARSE EXPLORATION ####################

        // using compressed representation of RNA, find similar objects
        // within this reduced search space (obviously the representation
        // should be as many-to-one as possible and yet be representative)

        // data acquisition
        ArrayList<ArrayList<String>> input1 = Readers.readMarnaFile(path);

        // representation and distance functions
        shapeAndSkeletonDist distance1 = new shapeAndSkeletonDist(1., 0.);
        shapeAndSkeletonDist distance2 = new shapeAndSkeletonDist(1, 1);
        GranularRepr converter = new GranularRepr(3);

        ArrayList<ArrayList<String>> converted = new ArrayList<>();
        for (ArrayList<String> list : input1) {
            HashSet<String> uniques = new HashSet<>();
            for (String dotBracket : list) {
                uniques.add(converter.call(dotBracket));
            }
            // add to the converted objects
            ArrayList<String> uniquesList = new ArrayList<>();
            for (String string : uniques) {
                uniquesList.add(string);
            }
            converted.add(uniquesList);
        }

        // create the problem instance (which also performs distance matrix calculations)
        ConsensusProblem<String> problem1 = new ConsensusProblem<>(converted, distance1);

        // actually solve
        boolean verbose = firstSolver.isVerbose();
        if (verbose) {
            System.err.println("Phase 1");
        }
        ArrayList<Solution> consensus1 = firstSolver.solve(problem1.getDistanceMatrix(), problem1.getRanges());

        // keep only unique solutions
        double numComparisons = input1.size() * (input1.size() - 1);  // n (n -1)
        ArrayList<ArrayList<String>> uniqueSolutions1 = new ArrayList<>();
        int i = 0;
        for (Solution solution : consensus1) {
            ArrayList<String> consensus = problem1.getObjects(solution.getGenes());
            Collections.sort(consensus);
            if (!uniqueSolutions1.contains(consensus)) {
                uniqueSolutions1.add(consensus);

                System.out.println("> solution " + i + " : " + solution.getScore() / numComparisons);
                for (String dotBracket : consensus) {
                    System.out.println(dotBracket);
                }
                System.out.println();
                i += 1;
            }
        }


        // ####################### PHASE 1.5: FILTERING #######################
        ArrayList<ArrayList<ArrayList<String>>> input2 = new ArrayList<>();

        // get the brackets
        for (ArrayList<String> solution : uniqueSolutions1) {
            ArrayList<ArrayList<String>> correspondingObjects = new ArrayList<>();
            for (String conv : solution) {
                correspondingObjects.add(new ArrayList<>(converter.getReverseMapping().get(conv)));
            }
            input2.add(correspondingObjects);
        }


        ArrayList<ConsensusProblem<String>> problems2 = new ArrayList<>();

        // instantiate the problems
        for (ArrayList<ArrayList<String>> input : input2) {

            problems2.add(new ConsensusProblem<>(input, distance2));
        }


        // #################### PHASE 2: REFINEMENT ####################
        // for each coarse consensus, refine by solving consensus
        // from selected objects
        if (verbose) {
            // update on the number of optimal trees found
            System.err.println(System.lineSeparator() + "Phase 2: String Edit Distance on Vienna Dot Bracket (" +
                    problems2.size() + ")");
        }

        // if too many output, make the solver silent and add a progress
        // bar over all tree consensus instead of individually
        if ((consensus1.size() > 10)) {
            verbose = false;
        }

        ArrayList<ArrayList<Solution>> consensus2 = new ArrayList<>();
        ProgressBar progress = new ProgressBar("", 40);
        for (i = 0; i != input2.size(); ++i) {
            consensus2.add(secondSolver.solve(problems2.get(i).getDistanceMatrix(), problems2.get(i).getRanges()));
            progress.update((float) i / consensus1.size());
        }
        progress.clean();

        if (verbose) {
            System.err.println();
        }


        // ################## STEP 3: OUTPUT SOLUTIONS ##################
        double score1;
        double bestScore2 = Double.POSITIVE_INFINITY;
        ArrayList<ArrayList<String>> uniqueSolutions2 = new ArrayList<>();
        i = 0;
        for (ArrayList<Solution> solutions : consensus2) {
            for (Solution solution : solutions) {
                ArrayList<String> consensus = problems2.get(i).getObjects(solution.getGenes());
                Collections.sort(consensus);
                if (!uniqueSolutions2.contains(consensus)) {
                    uniqueSolutions2.add(consensus);

                }

                if (solution.getScore() < bestScore2) {
                    bestScore2 = solution.getScore();
                }
            }
        }
    }
}
        /*
        for ()
                    System.out.println("> solution " + i + " : " + solution.getScore() / numComparisons);
                    for (String dotBracket : consensus) {
                        System.out.println(dotBracket);
                    }
                }

            }
            i += 1;
        }

        for (ArrayList<Solution> solutions : consensus2) {
            // figure out what the best scores are within each tree consensus
            score1 = consensus1.get(problem2Index).getScore() / num_comparisons;
            bestScore2 = Double.POSITIVE_INFINITY;

            for (Solution solution : solutions) {
                if (solution.getScore() < bestScore2) {
                    bestScore2 = solution.getScore();
                }
            }

            for (Solution solution : solutions) {
                if (solution.getScore() == bestScore2) {
                    // output it
                    double normalizedScore = bestScore2 / num_comparisons;
                    StringBuilder builder = new StringBuilder();
                    builder.append("> ");
                    builder.append(solutionIndex);       // index
                    builder.append(" ");
                    builder.append(score1);      // first score
                    builder.append(" ");
                    builder.append(normalizedScore); // normalized second score
                    builder.append(System.lineSeparator());
                    for (Integer gene : solution.getGenes()) {
                        String object = problems2.get(problem2Index).getObjects().get(gene);
                        builder.append(object);
                        builder.append(System.lineSeparator());
                    }
                    System.out.println(builder.toString());
                    solutionIndex += 1;
                }
            }
            problem2Index += 1;
        }
    }
}
*/
