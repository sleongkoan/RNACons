import java.util.ArrayList;

public class SubSolution extends Solution{

    private class RangeIndex
    {
        public int index;
        public int begin;
        public int end;
        public RangeIndex(int index_, int begin_, int end_)
        {
            index = index_;
            begin = begin_;
            end = end_;
        }
    }

    private ArrayList<RangeIndex> ranges;

    public SubSolution(ArrayList<Integer> genes_,
                       double score_,
                       ArrayList<RangeIndex> ranges_)
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

    public ArrayList<RangeIndex> getRanges() {
        return ranges;
    }

    public String toString()
    {
        // add the genes
        String stringRepresentation = "genes ";
        for (Integer i : getGenes())
        {
            stringRepresentation += i + " ";
        }
        stringRepresentation += System.lineSeparator();

        // add the score
        stringRepresentation += "score " + getScore() + System.lineSeparator();

        // add the remaining ranges
        stringRepresentation += "ranges" + System.lineSeparator();
        for( RangeIndex r : getRanges())
        {
            stringRepresentation += "[ " + r.index + " " + r.begin + " .. " + r.end + " ]";
            stringRepresentation += System.lineSeparator();
        }
        return stringRepresentation;
    }


    public Solution toSolution()
    {
        return new Solution(getGenes(), getScore());
    }
}