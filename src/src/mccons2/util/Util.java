package mccons2.util;

import java.util.*;

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

    public static String dotBracketToRTED(String dotBracket)
    {
        StringBuilder builder = new StringBuilder();
        for (char c: dotBracket.toCharArray())
        {
            if (c == '(')
            {
                builder.append("{ p");
            }
            else if (c == '.')
            {
                builder.append(" u ");
            }
            else
            {
                builder.append(" } ");
            }
        }
        return builder.toString().replace(" ", "");
    }


    public static <T> ArrayList<T> getSlice(ArrayList<T> array, int begin, int end) {
        ArrayList<T> slice = new ArrayList<>();
        for (int i = begin; i != end; ++i)
        {
            slice.add(array.get(i));
        }
        return slice;
    }

}
