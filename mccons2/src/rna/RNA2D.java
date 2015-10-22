package rna;


import java.util.ArrayList;

public class RNA2D {
    private final String dotBracket;
    private final String sequence;


    //region CONSTRUCTORS

    public RNA2D(String dotBracket, String sequence) {

        assert isValid2DForest(dotBracket);
        assert isValidRNASequence(sequence);

        this.dotBracket = dotBracket;
        this.sequence = sequence;
    }

    public RNA2D(RNA2D rna)
    {
        this.dotBracket = rna.getDotBracket();
        this.sequence = rna.getSequence();
    }
    //endregion


    //region INPUT CHECKERS
    /**
     * verifies that the arguments given to the constructor build a legal tree
     * @param dotBracket string representation, usually Vienna dot bracket
     * @return true if a valid tree is build from the arguments
     */
    public static boolean isValid2DForest(String dotBracket)
    {
        int counter = 0;
        for (char c : dotBracket.toCharArray())
        {
            if (c == '(')        // stack
            {
                counter += 1;
            }
            else if (c == ')')   // unstack
            {
                counter -= 1;
            }
            else if (c != '.')   // illegal character
            {
                return false;
            }
            if (counter < 0)     // left unbalanced
            {
                return false;
            }
        }
        return counter == 0;
    }


    /**
     * verifies if the RNA sequence given is composed of AUGC
     * @param sequence RNA sequence
     * @return true if it is a valid sequence of AUGC, false otherwise
     */
    public static boolean isValidRNASequence(String sequence)
    {
        boolean isValid = true;
        for (char c : sequence.toUpperCase().toCharArray())
        {
            if (!(c == 'A' || c =='U' || c == 'G' || c == 'C'))
            {
                isValid = false;
                break;
            }
        }
        return isValid;
    }
    //endregion


    //region ACCESS

    public String getDotBracket() {
        return new String(dotBracket);
    }

    public String getSequence() {
        return new String(sequence);
    }

    //endregion


    //region COMPARISON
    @Override
    public boolean equals(Object other)
    {
        boolean result = false;
        if (other instanceof RNA2D)
        {
            RNA2D that = (RNA2D) other;
            result = sequence.compareTo(that.getSequence()) == 0 && dotBracket.compareTo(that.getDotBracket()) == 0;
        }
        return result;
    }
    //endregion


}
