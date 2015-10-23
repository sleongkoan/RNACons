package distances;

public interface DistanceFunction<T1, T2> {

    Double get(T1 first, T2 second);
}


