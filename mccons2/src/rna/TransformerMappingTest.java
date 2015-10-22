package rna;

import junit.framework.TestCase;
import org.junit.Test;
import util.Readers;

import java.io.IOException;
import java.util.ArrayList;

public class TransformerMappingTest extends TestCase {

    @Test
    /**
     * verifies that the reverse mappings aren't empty (for elements which we know exist)
     */
    protected void testReverseMapping() {
        try {
            ArrayList<ArrayList<String>> data = Readers.readMarnaFile("data/examples/mccons_example.marna");
            Transformer<String, String> transformer = new GranularTransformer(3);
            TransformerMapping<String, String> mapper = new TransformerMapping<>(data.size());
            int classIndex = 0;
            for (ArrayList<String> list : data)
            {
                for (String dotBracket : list)
                {
                    String transformed = transformer.transform(dotBracket);
                    mapper.addMapping(classIndex, dotBracket, transformed);
                }
                classIndex += 1;
            }
        } catch (IOException e) {
            assertEquals("Could not find input file for test", true, false);
        }

    }
}