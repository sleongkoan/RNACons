package rna;

public class AbstractShapesTransformer extends Transformer<String, String> {

    private int level;

    public AbstractShapesTransformer(int level)
    {
        super("Abstract shapes transformer level" + level);
        assert (level == 1 || level == 3 || level == 5);
        this.level = level;
    }


    @Override
    public String transform(String obj) {
        return new AbstractShapes(obj, level).toString();
    }
}
