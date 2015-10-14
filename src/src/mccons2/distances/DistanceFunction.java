package mccons2.distances;

public interface DistanceFunction<T1, T2, T3> {

    T3 get(T1 first, T2 second);
}


