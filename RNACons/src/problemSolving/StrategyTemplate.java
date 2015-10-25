package problemSolving;

import distances.DistanceFunction;
import distances.StringEditDistance;
import distances.TreeEditDistance;
import representation.GranularTransformer;
import util.Pair;
import util.ProgressBar;
import util.RNAReaders;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;


public class StrategyTemplate {

    /**
     * useful class to hold the correspondence between repr1 -> repr2 when using a transformer
     * on RNA (during the coarse optimization phase of the StrategyTemplate)
     */
    static class TransformerMapping<A extends Comparable<A>, B extends Comparable<B>> {

        private ArrayList<ArrayList<Pair<A, B>>> mapping;

        public TransformerMapping(int numClasses) {
            mapping = new ArrayList<>();
            assert numClasses > 0;
            for (int i = 0; i != numClasses; ++i) {
                mapping.add(new ArrayList<Pair<A, B>>());
            }
        }


        public void addMapping(int classIndex, A from, B to) {
            mapping.get(classIndex).add(new Pair<>(from, to));
        }


        public ArrayList<A> getReverseMap(int index, B query) {
            assert (0 <= index && index < mapping.size());
            ArrayList<A> corresponding = new ArrayList<>();
            for (Pair<A, B> map : mapping.get(index)) {
                if (map.getSecond().compareTo(query) == 0) {
                    corresponding.add(map.getFirst());
                }
            }
            return corresponding;
        }


        private <T extends Comparable<T>> boolean contains(ArrayList<T> list, T toFind) {
            boolean contains = false;
            for (T element : list) {
                if (element.compareTo(toFind) == 0) {
                    contains = true;
                    break;
                }
            }
            return contains;
        }

        private <T extends Comparable<T>> ArrayList<T> getUniques(ArrayList<T> list) {
            ArrayList<T> uniques = new ArrayList<>();
            for (T obj : list) {
                if (!contains(uniques, obj)) {
                    uniques.add(obj);
                }
            }
            return uniques;

        }


        public ArrayList<A> getReverseMapUniques(int index, B query) {
            return getUniques(getReverseMap(index, query));

        }

        public ArrayList<A> getDomain() {
            ArrayList<A> domain = new ArrayList<>();
            for (int i = 0; i != mapping.size(); ++i) {
                for (Pair<A, B> map : mapping.get(i)) {
                    domain.add(map.getFirst());
                }
            }
            return domain;
        }


        public ArrayList<A> getDomainUniques() {
            return getUniques(getDomain());
        }

        public ArrayList<B> getCoDomain() {
            ArrayList<B> coDomain = new ArrayList<>();
            for (int i = 0; i != mapping.size(); ++i) {
                for (Pair<A, B> map : mapping.get(i)) {
                    coDomain.add(map.getSecond());
                }
            }
            return coDomain;
        }

        public ArrayList<B> getCoDomainUniques() {
            return getUniques(getCoDomain());
        }

    }


    //region DISTANCE DECLARATION
    static class compoundDistance implements DistanceFunction<String, String> {
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
     * @param path      path of the input file
     * @param strategy1 strategy for the coarse problem
     * @param strategy2 strategy for the refined problem
     */
    public static void applyStrategy(String path,
                                     AbstractStrategy strategy1,
                                     AbstractStrategy strategy2) throws IOException {

        //region COARSE EXPLORATION
        // using compressed representation of RNA, find similar objects
        // within this reduced search space (obviously the representation
        // should be as many-to-one as possible and yet be representative)

        // data acquisition
        final ArrayList<ArrayList<String>> rawData = RNAReaders.readMarnaFile(path);

        // representation and distance functions
        final compoundDistance distance1 = new compoundDistance(1., 0.);
        final compoundDistance distance2 = new compoundDistance(1, 1);

        // transform raw data to input and remember the mappings
        final ArrayList<ArrayList<String>> input1 = new ArrayList<>();
        final GranularTransformer converter = new GranularTransformer(3);
        TransformerMapping<String, String> input1Mapping = new TransformerMapping<>(rawData.size());

        for (int index = 0; index != rawData.size(); ++index) {

            ArrayList<String> list = rawData.get(index);
            HashSet<String> uniques = new HashSet<>();

            for (String dotBracket : list) {
                String transformed = converter.transform(dotBracket);
                uniques.add(transformed);

                // remember the mapping
                input1Mapping.addMapping(index, dotBracket, transformed);
            }
            // add to the converted objects
            ArrayList<String> uniquesList = new ArrayList<>();
            for (String string : uniques) {
                uniquesList.add(string);
            }
            input1.add(uniquesList);
        }

        // create the problem instance (which also performs distance matrix calculations)
        final Problem<String> problem1 = new Problem<>(input1, distance1);

        // actually solve
        boolean verbose = strategy1.isVerbose();
        if (verbose) {
            System.err.println("Phase 1");
        }
        ArrayList<Solution> consensus1 = strategy1.solve(problem1);

        // keep only unique solutions
        final double numComparisons = rawData.size() * (rawData.size() - 1);  // n (n -1)
        ArrayList<Double> coarseScores = new ArrayList<>();
        ArrayList<ArrayList<String>> uniqueSolutions1 = new ArrayList<>();

        for (Solution solution : consensus1) {
            ArrayList<String> consensus = problem1.getObjectsAtIndices(solution.getGenes());
            if (!uniqueSolutions1.contains(consensus)) {
                // add to unique solutions and remember the coarse score
                uniqueSolutions1.add(consensus);
                coarseScores.add(solution.getScore() / numComparisons);
            }
        }
        //endregion


        //region FILTERING
        ArrayList<ArrayList<ArrayList<String>>> input2 = new ArrayList<>();

        // get the brackets
        for (ArrayList<String> solution : uniqueSolutions1) {
            ArrayList<ArrayList<String>> correspondingObjects = new ArrayList<>();
            for (int classIndex = 0; classIndex != solution.size(); ++classIndex) {
                correspondingObjects.add(input1Mapping.getReverseMapUniques(classIndex, solution.get(classIndex)));
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
            consensus2.add(strategy2.solve(problems2.get(i)));
            progress.update((float) i / consensus1.size());
        }
        progress.clean();

        if (verbose) {
            System.err.println();
        }
        //endregion


        //region OUTPUT SOLUTIONS

        ArrayList<ArrayList<String>> uniqueSolutions2 = new ArrayList<>();
        int outputIndex = 0;
        for (int i = 0; i != consensus2.size(); ++i) {
            double score1 = coarseScores.get(i);

            ArrayList<Solution> solutions = consensus2.get(i);
            Problem<String> problem = problems2.get(i);
            uniqueSolutions2.add(problem1.getObjectsAtIndices(consensus1.get(i).getGenes()));

            for (Solution solution : solutions) {
                ArrayList<String> consensus = problem.getObjectsAtIndices(solution.getGenes());
                if (!uniqueSolutions2.contains(consensus)) {
                    uniqueSolutions2.add(consensus);
                    System.out.println();
                    System.out.printf("> %d; %.2f; %.2f" + System.lineSeparator(),outputIndex, score1, solution.getScore()/numComparisons);
                    for (String dotBracket : consensus) {
                        System.out.println(dotBracket);
                    }
                    outputIndex += 1;
                }
            }
        }
        //endregion
    }
}

