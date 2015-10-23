package representation;

public class GranularTransformer extends Transformer<String, String> {

    private int granularity;
    public GranularTransformer(int granularity)
    {
        super("Granular " + granularity);
        assert granularity > 0;
        this.granularity = granularity;
    }

    @Override
    public String transform(String obj) {
        return convenience.RNA2D.dotBracketToGranularTree(obj, granularity);
    }
}
