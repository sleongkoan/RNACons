package mccons.solvers;

import java.util.ArrayList;



public class Solution implements Comparable<Solution> {

    private ArrayList<Integer> genes;
    private double score;

    public void setGene(int index, int newValue) {
        genes.set(index, newValue);
    }

    /**
     * Constructor
     * @param genes_ index of the chosen genes in a table
     * @param score_ score of the current solution configuration (based on genes)
     */
    public Solution(ArrayList<Integer> genes_,
                    double score_)
    {
        genes = genes_;
        score = score_;
    }


    /**
     * Copy constructor
     * @param other solution to copy
     */
    public Solution(Solution other) {
        this(other.getGenes(), other.getScore());
    }


    public void setScore(double score) {
        this.score = score;
    }

    public double getScore() {

        return score;
    }

    public ArrayList<Integer> getGenes() {
        return genes;
    }

    public String toString()
    {
        String stringRepresentation = "[";
        for (int i =0 ; i != getGenes().size(); ++i)
        {
            stringRepresentation += getGenes().get(i) + " ";
        }
        stringRepresentation += " : " + getScore() + "]" + System.lineSeparator();
        return stringRepresentation;
    }


    public int compareTo(Solution other)
    {
        assert (other != null);
        return toString().compareTo(other.toString());
    }
}



