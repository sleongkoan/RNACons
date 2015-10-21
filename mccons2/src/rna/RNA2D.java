package rna;


import java.util.ArrayList;


/**
 * Node representation used for most trees in here
 */
class Node {

    // fields
    private ArrayList<Node> children;
    private Node parent;
    private char label;
    private int index;


    /**
     * Constructor
     * @param parent_ parent of the node, can be null
     * @param label_  label of the node, a char
     */
    public Node(Node parent_, char label_) {
        // constructor
        if (parent_ != null) {
            parent_.getChildren().add(this);
        }
        parent = parent_;
        label = label_;
        index = -1;
        children = new ArrayList<>();
    }


    public ArrayList<Node> getChildren() {
        return children;
    }

    public Node getParent() {
        return parent;
    }

    public char getLabel() {
        return label;
    }

    public int getIndex() {
        return index;
    }

    public void setChildren(ArrayList<Node> children) {
        this.children = children;
    }

    public void setParent(Node parent) {
        this.parent = parent;
    }

    public void setLabel(char label) {
        this.label = label;
    }

    public void setIndex(int index) {
        this.index = index;
    }
}




/**
 * Ordered labeled tree used to represent both RNA abstract shapes
 * and Vienna dot-bracket (planar ordered labeled tree)
 */

class Tree {

    private Node artificialRoot;
    private char nestingSymbol;
    private char closingSymbol;
    private char addNodeSymbol;

    /**
     * Constructor
     * Vienna dot bracket symbols =  ( ) .
     * Abstract shapes symbols = [ ] _
     *
     * @param stringRepresentation_ representation of the tree
     * @param nestingSymbol        symbol that causes nesting
     * @param closingSymbol        symbol that corresponds to going up in the tree
     * @param addNodeSymbol        symbol to add a node to the current node
     */
    public Tree(String stringRepresentation_,
                char nestingSymbol,
                char closingSymbol,
                char addNodeSymbol) {

        // assign the string representation and remember the symbols
        this.nestingSymbol = nestingSymbol;
        this.closingSymbol = closingSymbol;
        this.addNodeSymbol = addNodeSymbol;

        // create rooted tree (with artificial root)
        artificialRoot = new Node(null, this.nestingSymbol);
        Node position = artificialRoot;
        int index = 0;
        for (char c : stringRepresentation_.toCharArray()) {
            if (c == this.nestingSymbol)       // create new node and position goes down
            {
                position = new Node(position, this.nestingSymbol);
                position.setIndex(index);
                index += 1;
            } else if (c == this.closingSymbol)  // position goes up
            {
                position = position.getParent();
            } else if (c == this.addNodeSymbol) // add unpaired node, position stays same
            {
                new Node(position, this.addNodeSymbol);
                position.setIndex(index);
                index += 1;
            }
        }
    }


    /**
     * recursively go through the tree by post-order and add the labels to the list of labels
     * @param position node position in the tree being traversed
     * @param labelsList list of the labels already traversed (to which labels are added)
     */
    private void getPostOrderLabels(Node position, ArrayList<Character> labelsList)
    { //
        if (position.getLabel() == nestingSymbol)
        {
            labelsList.add(nestingSymbol);
            for (Node newPosition : position.getChildren())
            {
                getPostOrderLabels(newPosition, labelsList);
            }
            labelsList.add(closingSymbol);
        }
        else if (position.getLabel() == addNodeSymbol)
        {
            labelsList.add(addNodeSymbol);
        }
    }


    public String toString()
    {
        ArrayList<Character> symbolList = new ArrayList<>();
        for (Node position : artificialRoot.getChildren())
        {
            getPostOrderLabels(position, symbolList);
        }
        StringBuilder builder = new StringBuilder(symbolList.size());
        for (Character c  : symbolList)
        {
            builder.append(c);
        }
        return builder.toString();
    }

    public Node getRoot() {
        return artificialRoot;
    }
}


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
