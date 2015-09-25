package mccons.repr;

import java.util.ArrayList;

/**
 * mccons.Node representation used for most trees in here
 */
public class Node {

    // fields

    public void setChildren(ArrayList<Node> children) {
        this.children = children;
    }

    private ArrayList<Node> children;
    private char label;

    public void setIndex(int index) {
        this.index = index;
    }

    private int index;
    private Node parent;

    /**
     * Constructor
     *
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

    /**
     * @return array of the children nodes
     */
    public ArrayList<Node> getChildren() {
        return children;
    }

    /**
     * @return label of the current node
     */
    public char getLabel() {
        return label;
    }

    /**
     * @return index of the current node in post-order traversal
     */
    public int getIndex() {
        return index;
    }

    /**
     * @return parent of the current node, can be null if node is not rooted
     */
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
