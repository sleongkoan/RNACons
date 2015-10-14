package mccons2.rna;

import junit.framework.TestCase;
import org.junit.Test;

public class GranularBasePairTreeTest extends TestCase {


    @Test
    public void testGranularTransformation()
    {
        GranularBasePairTree tree = new GranularBasePairTree("((..)).(.)", 2);
        assertEquals("()()", tree.toString());

        tree = new GranularBasePairTree("(.)....((((....))))", 2);
        assertEquals("()(())", tree.toString());

        tree = new GranularBasePairTree("(.)....(((((....)))))", 2);
        assertEquals("()((()))", tree.toString());

        tree = new GranularBasePairTree("(.)....((((....))))", 3);
        assertEquals("()(())", tree.toString());

        tree = new GranularBasePairTree("(.)..(.)..()", 3);
        assertEquals("()()()", tree.toString());

        tree = new GranularBasePairTree("(.)..(.)..()", 30);
        assertEquals("()()()", tree.toString());


    }
}