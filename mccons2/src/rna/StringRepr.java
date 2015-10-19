package rna;

public class StringRepr extends RNAConverter{

    public StringRepr() {
        this.name = "String";
    }

    @Override
    protected String convert(String obj) {
        return obj;
    }
}
