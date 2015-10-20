package rna;

import java.util.ArrayList;


/**
 * mccons2.Node representation used for most trees in here
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

public class Tree {

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