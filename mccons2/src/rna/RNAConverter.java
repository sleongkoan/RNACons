package rna;

import java.util.HashSet;
import java.util.Hashtable;


/**
 * all calls to conversion should be memoized and remebered by reverse mapping
 */
public abstract class RNAConverter {
    protected Hashtable<String, HashSet<String>> reverseMapping;
    protected String name;

    RNAConverter()
    {
        this.reverseMapping = new Hashtable<>();
    }

    public Hashtable<String, HashSet<String>> getReverseMapping()
    {
        return reverseMapping;
    }
    public String getName(){ return name; }

    /**
     * call to convert an object, its result must be added to the internal reverse mapping
     * @param obj object to convert
     * @return converted string
     */
    protected abstract String convert(String obj);

    public String call(String dotBracket)
    {
        // convert the dot-bracket
        String result = convert(dotBracket);

        // add it to the internal mapping
        if (!reverseMapping.containsKey(result))
        {
            reverseMapping.put(result, new HashSet<String>());
        }
        reverseMapping.get(result).add(dotBracket);

        return result;
    }

}
