package mccons.repr;

import java.util.ArrayList;

/**
 * mccons.Node representation used for most trees in here
 */
public class Node {

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


    public void setIndex(int index) {
        this.index = index;
    }


    public void setChildren(ArrayList<Node> children) {
        this.children = children;
    }


    public ArrayList<Node> getChildren() {
        return children;
    }


    public char getLabel() {
        return label;
    }


    public int getIndex() {
        return index;
    }


    public Node getParent() {
        return parent;
    }


    /**
     * TODO: there is a better way to find them, but this works ok for now
     *
     * @return index of the leftmost descendent of the node
     */
    int getLeftmostDescendantIndex() {
        Node position = this;
        while (position.getChildren().size() > 0) {
            position = position.getChildren().get(0);
        }
        return position.getIndex();
    }
}
