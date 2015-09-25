package mccons.repr;

public final class UnitCostChar implements CostFunction<Character, Integer> {

    public Integer calculateCost(Character elem) {
        return 1;
    }
}
