package mccons.rna;

import java.util.ArrayList;


/**
 * mccons.Node representation used for most trees in here
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

public class OrderedRootedTree {

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
     * @param nestingSymbol_        symbol that causes nesting
     * @param closingSymbol_        symbol that corresponds to going up in the tree
     * @param addNodeSymbol_        symbol to add a node to the current node
     */
    public OrderedRootedTree(String stringRepresentation_,
                             char nestingSymbol_,
                             char closingSymbol_,
                             char addNodeSymbol_) {

        // assign the string representation and remember the symbols
        nestingSymbol = nestingSymbol_;
        closingSymbol = closingSymbol_;
        addNodeSymbol = addNodeSymbol_;

        // create rooted tree (with artificial root)
        artificialRoot = new Node(null, nestingSymbol);
        Node position = artificialRoot;

        for (char c : stringRepresentation_.toCharArray()) {
            if (c == nestingSymbol)       // create new node and position goes down
            {
                position = new Node(position, nestingSymbol);
            } else if (c == closingSymbol)  // position goes up
            {
                position = position.getParent();
            } else if (c == addNodeSymbol) // add unpaired node, position stays same
            {
                new Node(position, addNodeSymbol);
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