package mccons.repr;

public interface CostFunction<T1, T2> {

    T2 calculateCost(T1 elem);
}


