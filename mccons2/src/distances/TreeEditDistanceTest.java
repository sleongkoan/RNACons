package distances;

import junit.framework.TestCase;

public class TreeEditDistanceTest extends TestCase {

    public void testGet() throws Exception {
        TreeEditDistance dist = new TreeEditDistance(1., 1., 1.);
        String first = "()(((())))"; String second = "()((()))";
        assertEquals(1.0, dist.get(first, second));

    }
}