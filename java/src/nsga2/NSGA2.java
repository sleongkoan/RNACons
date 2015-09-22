package nsga2;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;



public final class NSGA2 {

    /**
     * shuffle an ArrayList using the Fisher-Yates shuffle algorithm
     * with no side effect on the list given
     *
     * @param list   ArrayList
     * @param <T>    type for the ArrayList elements
     * @param stream mccons.RngStream object to get the random numbers
     * @return shuffled arrayList
     */
    public static <T> ArrayList<T> fisherYatesShuffle(List<T> list, RngStream stream) {
        ArrayList<T> copied = new ArrayList<>(list);
        int j;
        T tmp;
        for (int i = copied.size() - 1; i != 0; i--) {
            j = stream.randInt(0, i);
            // exchange list[i] with list[j] and vice versa
            tmp = copied.get(i);
            copied.set(i, copied.get(j));
            copied.set(j, tmp);
        }
        return copied;
    }

    public static <T> ArrayList<T> selectWithoutReplacement(List<T> list, int numToSelect, RngStream stream) {
        assert (numToSelect <= list.size());
        ArrayList<T> copied = fisherYatesShuffle(list, stream);
        return new ArrayList<>(copied.subList(0, numToSelect));
    }


    /**
     * checks wether or not the list given is sorted
     *
     * @param list
     * @param <T>
     * @return
     */
    public static <T extends Comparable<T>> boolean isSorted(List<T> list) {
        for (int i = 1; i < list.size(); ++i) {
            if (list.get(i - 1).compareTo(list.get(i)) > 0) {
                return false;
            }
        }
        return true;
    }


    /**
     * O(n) deletion, taking advantage that both lists are sorted
     *
     * @param sortedList
     * @param sortedIndicesToDelete
     * @return
     */
    public static ArrayList<Integer> fastDelete(ArrayList<Integer> sortedList,
                                                ArrayList<Integer> sortedIndicesToDelete) {
        assert (isSorted(sortedList) && isSorted(sortedIndicesToDelete));
        ArrayList<Integer> filtered = new ArrayList<>();
        int deletionIndex = 0;
        final int numToDelete = sortedIndicesToDelete.size();
        for (int i : sortedList) {
            // catch up
            while ((sortedIndicesToDelete.get(deletionIndex) < i) && (deletionIndex < numToDelete)) {
                deletionIndex += 1;
            }
            if (i != sortedIndicesToDelete.get(deletionIndex)) {
                filtered.add(i);
            }

        }
        return filtered;
    }


    /**
     * used to tell wether first vector is "better" than the second
     * ([0, 0, 2]  > [0, 0, 1]) =  1
     * ([0, 0, 1] == [0, 1, 0]) =  0
     * ([1, 0, 1]  < [1, 1, 1]) = -1
     *
     * @param first  first list to compare
     * @param second second list to compare
     * @param <T>
     * @return 1 if first dominates, 0 if non dominant and -1 if first is dominated
     */
    public static <T extends Comparable<T>> int nonDominatedCompare(ArrayList<T> first, ArrayList<T> second) {
        assert (first.size() == second.size());

        boolean firstDominates = false;
        boolean secondDominates = false;
        T i;
        T j;
        int compare;
        for (int index = 0; index != first.size(); ++index) {
            i = first.get(index);
            j = second.get(index);
            compare = i.compareTo(j);
            if (compare < 0) {
                secondDominates = true;
            } else if (compare > 0) {
                firstDominates = true;
            }

            if (firstDominates && secondDominates) {
                return 0;
            }
        }

        if (firstDominates) {
            return 1;
        } else if (secondDominates) {
            return -1;
        } else {
            return 0;
        }
    }


    public static ArrayList<Integer> evaluateAgainstOthers(ArrayList<MultiObjSolution> population, int index) {
        ArrayList<Integer> dominatedBy = new ArrayList<>();
        ArrayList<Double> fitness = population.get(index).getScores();

        for (int i = 0; i != population.size(); ++i) {
            if (i != index) {
                if (nonDominatedCompare(population.get(i).getScores(), fitness) > 0) {
                    dominatedBy.add(i);
                }
            }
        }
        return dominatedBy;
    }


    /**
     * sort population into nondominating fronts (best to worst) until
     * at least half the original number of individuals is put in a front
     * @param population
     * @return
     */
    public static ArrayList<ArrayList<Integer>> nonDominatedSort(ArrayList<MultiObjSolution> population)
    {
        ArrayList<Pair<Integer, ArrayList<Integer>>> dominationInformation = new ArrayList<>();
        for (int index = 0; index != population.size(); ++index)
        {
            dominationInformation.add(new Pair<>(index, evaluateAgainstOthers(population, index)));
        }

        // find nondominated individuals and separate them from the rest
        // until there are at least half of the population in them
        int cutoff = (int) Math.ceil(population.size() / 2.0);
        ArrayList<Integer> currentFrontIndices = new ArrayList<>();
        int index;
        ArrayList<Integer> dominatedBy;
        ArrayList<ArrayList<Integer>> dominationFronts = new ArrayList<>();
        ArrayList<Pair<Integer, ArrayList<Integer>>> dominated = new ArrayList<>();

        while (dominationInformation.size() > cutoff){
            for (Pair<Integer, ArrayList<Integer>> information : dominationInformation){
                index = information.getFirst();
                dominatedBy = information.getSecond();

                if (dominatedBy.size() == 0){ // nondominated
                    currentFrontIndices.add(index);
                }
                else // dominated
                {
                    dominated.add(new Pair<>(index, dominatedBy));
                }
            }
            // push the current front to the result
            dominationFronts.add(currentFrontIndices);
            dominationInformation = new ArrayList<>();
            for (Pair<Integer, ArrayList<Integer>> information : dominated)
            {
                ArrayList<Integer> subtracted = fastDelete(information.getSecond(), currentFrontIndices);
                dominationInformation.add(new Pair<>(information.getFirst(), subtracted));
            }
        }
    return dominationFronts;
    }



    private static <T> ArrayList<T> getIndices(ArrayList<T> list, ArrayList<Integer> indices)
    {
        ArrayList<T> subList = new ArrayList<>();
        for (int index : indices)
        {
            subList.add(list.get(index));
        }
        return subList;
    }


    /**
     * crowding distance measures the proximity of a solution to its neighbors
     * it is used to preserve diversity, later in the algorithm
     */
    public static void calculateCrowdingDistance(ArrayList<MultiObjSolution> population,
                                                 ArrayList<Integer> frontIndices,
                                                 int frontIndex)
    {
        ArrayList<MultiObjSolution> front = getIndices(population, frontIndices);
        ArrayList<ArrayList<Double>> fitnesses = new ArrayList<>();
        for (MultiObjSolution solution : front)
        {
            fitnesses.add(solution.getScores());
        }

        // map fitness => crowding_distance
        Hashtable<ArrayList<Double>, Pair<Integer, Double>> fitnessToCrowding = new Hashtable<>();
        for (ArrayList<Double> fitness : fitnesses)
        {
            fitnessToCrowding.put(fitness, new Pair<>(frontIndex, 0.0));
        }
        ArrayList<ArrayList<Double>> fitnessKeys = new ArrayList<>(fitnessToCrowding.keys());
        int fitnessLength = fitnessKeys.get(0).size();

        ArrayList<ArrayList<Double>> sortedByObjectives = new ArrayList<>();
        ArrayList<Double> objectiveRange = new ArrayList<>();

    }
/*


  # sort in decreasing order the fitness vectors for each objective
  sorted_by_objective::Vector{Vector{B}} = Vector{B}[]
  objective_range::Vector{Number} = Number[]

  for i = 1:fitness_length
    sorted = sort(fitness_keys, by = x->x[i], rev = true)
    push!(objective_range, sorted[1][i] - sorted[end][i])
    push!(sorted_by_objective, sorted)
  end

  # assign infinite crowding distance to maximum and
  # minimum fitness of each objective
  map(x->fitness_to_crowding[x[end]] = (front_index, Inf), sorted_by_objective)
  map(x->fitness_to_crowding[x[1]]   = (front_index, Inf), sorted_by_objective)

  # assign crowding crowding_distances to the other
  # fitness vectors for each objectives
  for i = 1:fitness_length
    # edge case here! if range == 0, 0 / 0 will give NaN, ignore the objective in such case
    # in DEAP, this is treated using
    # if crowd[-1][0][i] == crowd[0][0][i]:
    #        continue
    if objective_range[i] != 0
      for j = (2:(length(fitness_keys)-1))
        crowding_distance = fitness_to_crowding[sorted_by_objective[i][j]][2]
        crowding_distance += ((sorted_by_objective[i][j-1][i] - sorted_by_objective[i][j+1][i]) / objective_range[i])
        fitness_to_crowding[sorted_by_objective[i][j]] = (front_index, crowding_distance)
      end
    end
  end

  fitness_to_crowding
end
 */
}