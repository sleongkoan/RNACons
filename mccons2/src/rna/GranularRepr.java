package rna;

import java.util.Hashtable;

public class GranularRepr extends RNAConverter {

    public int granularity;

    public GranularRepr(int granularity) {
        assert granularity > 0;
        this.granularity = granularity;
        this.name = "granular " + granularity;
    }

    @Override
    public String convert(String obj) {
        String converted = new GranularBasePairTree(obj, granularity).toString();
        return converted;
    }
}
