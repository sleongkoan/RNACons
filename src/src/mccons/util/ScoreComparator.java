package mccons.util;

import java.util.Comparator;

public class ScoreComparator<T> implements Comparator<Pair <T, Double>> {
    @Override
    public int compare(Pair<T, Double> p1, Pair<T, Double> p2)
    {
        return p1.getSecond().compareTo(p2.getSecond());
    }

}
