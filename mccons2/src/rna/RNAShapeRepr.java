package rna;

public class RNAShapeRepr extends RNAConverter {

    private int level;

    public RNAShapeRepr( int level) {
        assert (level == 1 || level == 3 || level == 5);
        this.level = level;
        this.name = "Abstract shapes level " + level;
    }

    @Override
    protected String convert(String obj) {
        String converted = RNAshapes.dotBracketToAbstractShape(obj, level);
        converted = converted.replace("[", "(").replace("]", ")").replace("_", ".");
        return converted;
    }
}
