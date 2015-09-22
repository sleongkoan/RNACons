package mccons;

import java.util.Comparator;

public class Pair<T1, T2>{

    private T1 first;
    private T2 second;

    public Pair(T1 first_, T2 second_){
        first = first_;
        second = second_;
    }

    public T1 getFirst() {
        return first;
    }

    public T2 getSecond() {
        return second;
    }

}

class WidthComparator implements Comparator<Pair<Integer, Integer>>{
    @Override
    public int compare(Pair<Integer, Integer> p1, Pair<Integer, Integer> p2)
    {
        Integer width1 = p1.getSecond() - p1.getFirst();
        Integer width2 = p2.getSecond() - p2.getFirst();
        return width1.compareTo(width2);
    }
}


class ScoreComparator<T> implements Comparator<Pair <T, Double>>{
    @Override
    public int compare(Pair<T, Double> p1, Pair<T, Double> p2)
    {
        return p1.getSecond().compareTo(p2.getSecond());
    }

}
