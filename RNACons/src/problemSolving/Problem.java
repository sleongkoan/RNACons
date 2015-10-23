package problemSolving;

import distances.DistanceFunction;
import util.Pair;
import util.Util;

import java.util.ArrayList;
import java.util.Hashtable;

class Problem<T> {

    ArrayList<T> objects;
    ArrayList<Pair<Integer, Integer>> ranges;
    double[][] distanceMatrix;


    public Problem(ArrayList<ArrayList<T>> objects, DistanceFunction<T, T> distanceFunction) {
        this.objects = Util.flatten(objects);
        for (ArrayList<T> list : objects)
        {
            assert list.size() != 0;
        }
        this.ranges = getRanges(objects);
        this.distanceMatrix = getDistanceMatrix(objects, distanceFunction);
    }


    /**
     *
     * @param data
     * @param <T>
     * @return
     */
    private static <T> ArrayList<Pair<Integer, Integer>> getRanges(ArrayList<ArrayList<T>> data) {
        ArrayList<Pair<Integer, Integer>> ranges = new ArrayList<>();

        int count = 0;
        for (ArrayList<T> L : data) {
            Pair<Integer, Integer> r = new Pair<>(count, count + L.size());
            ranges.add(r);
            count += L.size();
        }
        return ranges;
    }


    /**
     *
     * @param data
     * @param distanceFunction
     * @param <T>
     * @return
     */
    private static <T> double[][] getDistanceMatrix(ArrayList<ArrayList<T>> data,
                                                    DistanceFunction<T, T> distanceFunction) {
        ArrayList<T> objects = new ArrayList<>();
        for (ArrayList<T> list : data)
        {
            objects.addAll(list);
        }

        // fill the distance matrix
        double[][] distanceMatrix = new double[objects.size()][objects.size()];
        for (int i = 0; i != distanceMatrix.length; ++i) {
            for (int j = 0; j != distanceMatrix.length; ++j) {
                distanceMatrix[i][j] = Double.POSITIVE_INFINITY;
            }
        }

        // memorize distance calculations
        ArrayList<T> uniqueObjects = new ArrayList<>();
        Hashtable<T, Integer> objToIndex = new Hashtable<>();
        for (T obj : objects) {
            if (!(uniqueObjects.contains(obj))) {
                uniqueObjects.add(obj);
                objToIndex.put(obj, uniqueObjects.size() - 1);
            }
        }

        double[][] condensedDistances = new double[uniqueObjects.size()][uniqueObjects.size()];
        double distance;

        for (int i = 0; i != uniqueObjects.size() - 1; ++i) {
            T obj1 = uniqueObjects.get(i);
            for (int j = i + 1; j != uniqueObjects.size(); ++j) {
                T obj2 = uniqueObjects.get(j);

                // calculate and assign the distance
                distance = distanceFunction.get(obj1, obj2);
                condensedDistances[i][j] = distance;
                condensedDistances[j][i] = distance;
            }
        }

        // fill the distance matrix
        int index1, index2;
        for (int i = 0; i != objects.size() - 1; ++i) {
            index1 = objToIndex.get(objects.get(i));
            for (int j = i + 1; j != objects.size(); ++j) {
                index2 = objToIndex.get(objects.get(j));
                distance = condensedDistances[index1][index2];
                distanceMatrix[i][j] = distance;
                distanceMatrix[j][i] = distance;
            }
        }

        // fill the diagonal with zeros
        for (int i = 0; i < distanceMatrix.length; ++i) {
            distanceMatrix[i][i] = 0.;
        }
        return distanceMatrix;
    }

    public ArrayList<T> getObjectsAtIndices(ArrayList<Integer> indices)
    {
        ArrayList<T> objectsAtIndices = new ArrayList<>();
        for (Integer index : indices)
        {
            if (index >= objects.size())
            {
                System.out.println(indices.toString());
            }
            objectsAtIndices.add(objects.get(index));
        }
        return objectsAtIndices;
    }


    public ArrayList<Pair<Integer, Integer>> getRanges() {
        return ranges;
    }

    public double[][] getDistanceMatrix() {
        return distanceMatrix;
    }
}
