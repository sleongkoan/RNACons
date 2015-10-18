package util;

import java.util.Comparator;

public class WidthComparator implements Comparator<Pair<Integer, Integer>> {
    @Override
    public int compare(Pair<Integer, Integer> p1, Pair<Integer, Integer> p2)
    {
        Integer width1 = p1.getSecond() - p1.getFirst();
        Integer width2 = p2.getSecond() - p2.getFirst();
        return width1.compareTo(width2);
    }
}
