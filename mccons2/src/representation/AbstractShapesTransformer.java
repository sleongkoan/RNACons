package representation;

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
        return convenience.RNA2D.dotBracketToAbstractShape(obj, level);
    }
}
