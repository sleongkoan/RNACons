
public interface CostFunction<T1, T2> {

    T2 calculateCost(T1 elem);
}


final class unitCostChar implements CostFunction<Character, Integer> {

    public Integer calculateCost(Character elem) {
        return 1;
    }
}

