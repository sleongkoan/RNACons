package mccons.rna;

import junit.framework.TestCase;

public class OrderedRootedTreeTest extends TestCase {

    /**
     * verifies that the arguments given to the constructor build a legal tree
     * @param stringRepresentation_ string representation, usually Vienna dot bracket
     * @param nestingSymbol_ '(' for dot bracket
     * @param closingSymbol_ ')' for dot bracket
     * @param addNodeSymbol_ '.' for dot bracket
     * @return true if a valid tree is build from the arguments
     */
    public static boolean checkTree(String stringRepresentation_,
                                    char nestingSymbol_, char closingSymbol_, char addNodeSymbol_)
    {
        int counter = 0;
        char[] charRepresentation = stringRepresentation_.toCharArray();

        for (char c : charRepresentation)
        {
            if (c == nestingSymbol_)               // stack
            {
                counter += 1;
            }
            else if (c == closingSymbol_)          // unstack
            {
                counter -= 1;
            }
            else if (c != addNodeSymbol_)          // illegal character
            {
                return false;
            }
            if (counter < 0)            // left unbalanced
            {
                return false;
            }
        }
        return counter == 0;
    }

}