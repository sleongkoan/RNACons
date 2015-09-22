package mccons;

import java.util.ArrayList;

public class SubSolution extends Solution{
    

    private ArrayList<Pair<Integer, Integer>> ranges;


    public SubSolution(ArrayList<Integer> genes_,
                       double score_,
                       ArrayList<Pair<Integer, Integer>> ranges_)
    {
        super(genes_, score_);
        ranges = ranges_;
    }


    public SubSolution(SubSolution other)
    {
        this(new ArrayList<>(other.getGenes()),
                other.getScore(),
                new ArrayList<>(other.getRanges()));
    }


    public ArrayList<Pair<Integer, Integer>> getRanges() {
        return ranges;
    }


    public String toString()
    {
        // add the genes
        StringBuilder builder = new StringBuilder();
        builder.append("genes ");
        for (Integer i : getGenes())
        {
            builder.append(i + " ");
        }
        builder.append(System.lineSeparator());

        // add the score
        builder.append("score " + getScore() + System.lineSeparator());

        // add the remaining ranges
        builder.append("ranges" + System.lineSeparator());
        for( Pair<Integer, Integer> r : getRanges())
        {
            builder.append("[ " + r.getFirst() + " .. " + r.getSecond() + " ]");
            builder.append(System.lineSeparator());
        }
        return builder.toString();
    }


    public Solution toSolution()
    {
        return new Solution(getGenes(), getScore());
    }
}

