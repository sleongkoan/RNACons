package mccons.repr.distances;

public final class EqualCharDistance implements DistanceFunction<Character, Character, Integer> {

    public Integer distance(Character first, Character second) {
        if (first.compareTo(second) == 0) {
            return 0;
        } else {
            return 2;
        }
    }
}
