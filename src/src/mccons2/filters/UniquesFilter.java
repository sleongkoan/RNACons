package mccons2.filters;


import java.util.ArrayList;
import java.util.List;


/**
 * Simple filter used applied on lists of objects implementing a meaningful equals() function.
 * @param <T> type of the objects
 */
public class UniquesFilter<T extends Comparable<T>> implements Filter<T>{

    @Override
    public List<T> filter(List<T> inputList) {
        ArrayList<T> uniques = new ArrayList<>();
        for (T elem : inputList)
        {
            if(!uniques.contains(elem))
            {
                uniques.add(elem);
            }
        }
        return uniques;
    }


}
