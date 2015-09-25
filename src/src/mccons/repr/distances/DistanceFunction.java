package mccons.repr.distances;

public interface DistanceFunction<T1, T2, T3> {

    T3 distance(T1 first, T2 second);
}


