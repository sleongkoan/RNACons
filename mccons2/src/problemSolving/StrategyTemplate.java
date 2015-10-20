package problemSolving;

import distances.DistanceFunction;
import distances.StringEditDistance;
import distances.TreeEditDistance;
import util.ProgressBar;
import util.Readers;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;


public class StrategyTemplate {


    //region DISTANCE DECLARATION
    static class compoundDistance implements DistanceFunction<String, String, Double> {
        public final double weight1, weight2;

        compoundDistance(double weight1, double weight2) {
            this.weight1 = weight1;
            this.weight2 = weight2;
        }

        public Double get(String dotBracket1, String dotBracket2) {

            TreeEditDistance dist1 = new TreeEditDistance(1, 1, 1);
            StringEditDistance dist2 = new StringEditDistance();

            Double skeletonDistance = dist1.get(dotBracket1, dotBracket2);
            Double stringDistance = dist2.get(dotBracket1, dotBracket2);

            return (weight1 * skeletonDistance) + (weight2 * stringDistance);
        }
    }
    //endregion


    /**
     * given a strategy and a problem instance, apply the strategy
     *
     * @param path         path of the input file
     * @param firstSolver  strategy for the coarse problem
     * @param secondSolver strategy for the refined problem
     */
    public static void applyStrategy(String path,
                                     AbstractStrategy firstSolver,
                                     AbstractStrategy secondSolver) throws IOException {

        //region COARSE EXPLORATION
        // using compressed representation of RNA, find similar objects
        // within this reduced search space (obviously the representation
        // should be as many-to-one as possible and yet be representative)

        // data acquisition
        final ArrayList<ArrayList<String>> rawData = Readers.readMarnaFile(path);

        // representation and distance functions
        final compoundDistance distance1 = new compoundDistance(1., 0.);
        final compoundDistance distance2 = new compoundDistance(1, 1);
        final GranularRepr converter = new GranularRepr(3);

        final ArrayList<ArrayList<String>> input1 = new ArrayList<>();
        final ArrayList<HashSet<String>> input1Sets = new ArrayList<>();
        for (ArrayList<String> list : rawData) {
            HashSet<String> uniques = new HashSet<>();
            for (String dotBracket : list) {
                uniques.add(converter.call(dotBracket));
            }
            // add to the converted objects
            ArrayList<String> uniquesList = new ArrayList<>();
            for (String string : uniques) {
                uniquesList.add(string);
            }
            input1.add(uniquesList);
            input1Sets.add(new HashSet<>(uniquesList));
        }

        // create the problem instance (which also performs distance matrix calculations)
        final Problem<String> problem1 = new Problem(input1, distance1);

        // actually solve
        boolean verbose = firstSolver.isVerbose();
        if (verbose) {
            System.err.println("Phase 1");
        }
        ArrayList<Solution> consensus1 = firstSolver.solve(problem1);

        // keep only unique solutions
        final double numComparisons = rawData.size() * (rawData.size() - 1);  // n (n -1)
        ArrayList<ArrayList<String>> uniqueSolutions1 = new ArrayList<>();

        for (Solution solution : consensus1) {
            ArrayList<String> consensus = problem1.getObjectsAtIndices(solution.getGenes());
            Collections.sort(consensus);
            if (!uniqueSolutions1.contains(consensus)) {
                uniqueSolutions1.add(consensus);

                System.out.println(System.lineSeparator() + "> " + solution.getScore() / numComparisons);
                for (String dotBracket : consensus) {
                    System.out.println(dotBracket);
                }
                System.out.println();
            }
        }
        //endregion


        //region FILTERING
        ArrayList<ArrayList<ArrayList<String>>> input2 = new ArrayList<>();

        // get the brackets
        for (ArrayList<String> solution : uniqueSolutions1) {
            ArrayList<ArrayList<String>> correspondingObjects = new ArrayList<>();
            for (int i = 0; i != solution.size(); ++i)
            {
            }
            for (String conv : solution) {
                correspondingObjects.add(new ArrayList<>(converter.getReverseMapping().get(conv)));
            }
            input2.add(correspondingObjects);
        }


        ArrayList<Problem<String>> problems2 = new ArrayList<>();

        // instantiate the problems
        for (ArrayList<ArrayList<String>> input : input2) {

            problems2.add(new Problem<>(input, distance2));
        }
        //endregion


        //region FINE EXPLORATION
        // for each coarse consensus, refine by solving consensus
        // from selected objects
        if (verbose) {
            // update on the number of optimal trees found
            System.err.println(System.lineSeparator() + "Phase 2: String Edit Distance on Vienna Dot Bracket (" +
                    problems2.size() + ")");
        }

        // if too many output, make the strategy silent and add a progress
        // bar over all tree consensus instead of individually
        if ((consensus1.size() > 10)) {
            verbose = false;
        }

        ArrayList<ArrayList<Solution>> consensus2 = new ArrayList<>();
        ProgressBar progress = new ProgressBar("", 40);
        for (int i = 0; i != input2.size(); ++i) {
            consensus2.add(secondSolver.solve(problems2.get(i)));
            progress.update((float) i / consensus1.size());
        }
        progress.clean();

        if (verbose) {
            System.err.println();
        }
        //endregion


        //region OUTPUT SOLUTIONS
        double bestScore2 = Double.POSITIVE_INFINITY;
        ArrayList<ArrayList<String>> uniqueSolutions2 = new ArrayList<>();

        for (int i = 0; i != consensus2.size(); ++i) {
            ArrayList<Solution> solutions = consensus2.get(i);
            Problem problem = problems2.get(i);
            uniqueSolutions2.add(problem1.getObjectsAtIndices(consensus1.get(i).getGenes()));
            for (Solution solution : solutions) {
                ArrayList<String> consensus = problem.getObjectsAtIndices(solution.getGenes());
                Collections.sort(consensus);
                if (!uniqueSolutions2.contains(consensus)) {
                    uniqueSolutions2.add(consensus);

                }

                if (solution.getScore() < bestScore2) {
                    bestScore2 = solution.getScore();
                }
            }
        }
        for (ArrayList<String> solution : uniqueSolutions2)
        {
            System.out.println(System.lineSeparator() + "> ");
            for (String db : solution)
            {
                System.out.println(db);
            }

        }
        //endregion
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
                        String object = problems2.get(problem2Index).getElemsAtIndices().get(gene);
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
