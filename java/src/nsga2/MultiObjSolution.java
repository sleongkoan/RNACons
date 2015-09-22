package nsga2;

import mccons.Solution;

import java.util.ArrayList;


public class MultiObjSolution extends Solution {


    private ArrayList<Double> scores;


    public MultiObjSolution(ArrayList<Integer> genes_,
                       ArrayList<Double> scores_)
    {
        super(genes_, Double.POSITIVE_INFINITY);
        scores = scores_;
    }


    public MultiObjSolution(MultiObjSolution other)
    {
        this(new ArrayList<>(other.getGenes()), other.getScores());
    }


    public ArrayList<Double> getScores() {
        return scores;
    }


    public String toString()
    {
        // add the genes
        StringBuilder builder = new StringBuilder();
        builder.append("genes: ");
        for (Integer i : getGenes())
        {
            builder.append(i);
            builder.append(" ");
        }
        builder.append(System.lineSeparator());

        // add the score
        builder.append("score: ");
        for (Double score : getScores())
        {
            builder.append(score);
            builder.append(" ");
        }
        builder.append(System.lineSeparator());

        // add the remaining ranges
        builder.append("ranges");
        builder.append(System.lineSeparator());

        return builder.toString();
    }
}

