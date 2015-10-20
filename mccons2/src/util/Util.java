package util;

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
            else if (c == ')')
            {
                builder.append(" } ");
            }
        }
        return builder.toString().replace(" ", "");
    }

    /**
     *
     * @param tree
     * @return wether or not it is a tree
     */
    public static boolean isTree(String tree)
    {
        char[] skeleton = tree.replace(".", "").toCharArray();
        if(skeleton[0] != '(')
        {
            return false;
        }
        int counter = 0;
        int length = skeleton.length;
        int i = 0;
        for (char c : skeleton)
        {
            if (c == '(')
            {
                counter += 1;
            }
            else
            {
                counter -= 1;
            }
            i+=1;
            if (i != length && counter == 0)
            {
                return false;
            }
        }
        return true;
    }


    public static <T> ArrayList<T> getSlice(ArrayList<T> array, int begin, int end) {
        ArrayList<T> slice = new ArrayList<>();
        for (int i = begin; i != end; ++i)
        {
            slice.add(array.get(i));
        }
        return slice;
    }


    public static <T> ArrayList<T> flatten(ArrayList<ArrayList<T>> nested)
    {
        ArrayList<T> flat = new ArrayList<>();
        for (ArrayList<T> list : nested)
        {
            flat.addAll(list);
        }
        return flat;
    }

}
