package optimization;

import distances.DistanceFunction;
import util.Pair;

import java.util.ArrayList;
import java.util.Hashtable;

public class ConsensusProblem<T> {

    public ArrayList<T> getObjects() {
        return objects;
    }

    public ArrayList<Pair<Integer, Integer>> getRanges() {
        return ranges;
    }

    public double[][] getDistanceMatrix() {
        return distanceMatrix;
    }

    // fields
    private ArrayList<T> objects;
    private ArrayList<Pair<Integer, Integer>> ranges;
    private double[][] distanceMatrix;

    public ConsensusProblem(ArrayList<ArrayList<T>> data_, DistanceFunction<T, T, Double> distanceFun) {
        objects = new ArrayList<>();
        ranges = new ArrayList<>();
        int count = 0;
        for (ArrayList<T> L : data_) {
            for (T obj : L) {
                objects.add(obj);
            }
            Pair<Integer, Integer> r = new Pair<>(count, count + L.size());
            ranges.add(r);
            count += L.size();
        }

        // fill the get matrix
        distanceMatrix = new double[objects.size()][objects.size()];
        for (int i = 0; i != distanceMatrix.length; ++i) {
            for (int j = 0; j != distanceMatrix.length; ++j) {
                distanceMatrix[i][j] = Double.POSITIVE_INFINITY;
            }
        }


        // memoize get calculations
        ArrayList<T> uniqueObjects = new ArrayList<>();
        Hashtable<T, Integer> objToIndex = new Hashtable<>();
        for (T obj : objects) {
            if (!(uniqueObjects.contains(obj))) {
                uniqueObjects.add(obj);
                objToIndex.put(obj, uniqueObjects.size() - 1);
            }
        }


        double[][] condensed_distances = new double[uniqueObjects.size()][uniqueObjects.size()];
        double distance;

        for (int i = 0; i != uniqueObjects.size() - 1; ++i) {
            T obj1 = uniqueObjects.get(i);
            for (int j = i + 1; j != uniqueObjects.size(); ++j) {
                T obj2 = uniqueObjects.get(j);

                // calculate and assign the get
                distance = distanceFun.get(obj1, obj2);
                condensed_distances[i][j] = distance;
                condensed_distances[j][i] = distance;
            }
        }

        // fill the get matrix
        int index1, index2;
        for (int i = 0; i != objects.size() - 1; ++i) {
            index1 = objToIndex.get(objects.get(i));
            for (int j = i + 1; j != objects.size(); ++j) {
                index2 = objToIndex.get(objects.get(j));
                distance = condensed_distances[index1][index2];
                distanceMatrix[i][j] = distance;
                distanceMatrix[j][i] = distance;
            }
        }

        // fill the diagonal with zeros
        for (int i = 0; i < distanceMatrix.length; ++i) {
            distanceMatrix[i][i] = 0.;
        }
    }

    public ArrayList<T> getObjects(ArrayList<Integer> genes) {
        ArrayList<T> correspondingObjects = new ArrayList<>();
        for (int index : genes) {
            correspondingObjects.add(getObjects().get(index));
        }
        return correspondingObjects;
    }
}

