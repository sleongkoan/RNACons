package mccons.util;

import java.util.List;

public final class Util {

    public static <T extends Comparable<T>> boolean isSorted(List<T> list) {
        boolean sorted = true;
        for (int i = 1; i != list.size(); ++i) {
            if (list.get(i - 1).compareTo(list.get(i)) > 0) {
                sorted = false;
                break;
            }
        }
        return sorted;
    }
}
