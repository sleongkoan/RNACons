package rna;


/**
 * Abstract transformer class. All RNA representation transformations are done through classes
 * inheriting this.
 */
public abstract class Transformer<A, B> {
    protected String name;
    Transformer(String name)
    {
        this.name = name;
    }

    public String getName(){ return name; }

    /**
     * call to convert an object, its result must be added to the internal reverse mapping
     * @param obj object to convert
     * @return converted string
     */
    public abstract B transform(A obj);

}
