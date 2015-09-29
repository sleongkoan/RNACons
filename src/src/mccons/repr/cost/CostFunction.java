package mccons.repr.cost;

public interface CostFunction<T1, T2> {

    T2 get(T1 elem);
}

