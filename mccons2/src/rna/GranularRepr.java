package rna;

public class GranularRepr extends RNAConverter {

    public int granularity;

    public GranularRepr(String name, int granularity) {
        this.granularity = granularity;
        this.name = name;
    }

    @Override
    protected String convert(String obj) {
        String converted = new GranularBasePairTree(obj, granularity).toString();
        return converted;
    }
}
