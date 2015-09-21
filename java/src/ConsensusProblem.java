
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
    private ArrayList< Pair<Integer, Integer> > ranges;
    private double[][] distanceMatrix;

     public ConsensusProblem(ArrayList< ArrayList<T> > data_, DistanceFunction<T, T, Double> distanceFun)
    {
        objects = new ArrayList<>();
        ranges = new ArrayList<>();
        int count = 0;
        for (ArrayList<T> L : data_)
        {
            for (T obj : L)
            {
                objects.add(obj);
            }
            Pair<Integer, Integer> r = new Pair<>(count, count + L.size());
            ranges.add(r);
            count += L.size();
        }

        // fill the distance matrix
        distanceMatrix = new double[objects.size()][objects.size()];
        for (int i = 0; i != distanceMatrix.length; ++i) {
            for (int j = 0; j != distanceMatrix.length; ++j) {
                distanceMatrix[i][j] = Double.POSITIVE_INFINITY;
            }
        }


        // memoize distance calculations
        ArrayList<T> unique_objects = new ArrayList<>();
        Hashtable<T, Integer> obj_to_index = new Hashtable<>();
        for (T obj : objects)
        {
            if (! (unique_objects.contains(obj)))
            {
                unique_objects.add(obj);
                obj_to_index.put(obj, unique_objects.size() -1);
            }
        }

        double[][] condensed_distances = new double[unique_objects.size()][unique_objects.size()];
        double distance;

        for (int i = 0; i != unique_objects.size() - 1; ++i)
        {
            T obj1 = unique_objects.get(i);
            for (int j = i + 1; j != unique_objects.size(); ++j)
            {
                T obj2 = unique_objects.get(j);
                // calculate and assign the distance
                distance = distanceFun.distance(obj1, obj2);
                condensed_distances[i][j] = distance;
                condensed_distances[j][i] = distance;
            }
        }

        // fill the distance matrix
        int index1, index2;
        for (int i = 0; i != objects.size() - 1; ++i)
        {
            index1 = obj_to_index.get(objects.get(i));
            for (int j = i + 1; j != objects.size(); ++j)
            {
                index2 = obj_to_index.get(objects.get(j));
                distance = condensed_distances[index1][index2];
                distanceMatrix[i][j] = distance;
                distanceMatrix[j][i] = distance;
            }
        }

        // fill the diagonal with zeros
        for (int i = 0; i < distanceMatrix.length; ++i)
        {
            distanceMatrix[i][i] = 0.;
        }
    }
}






