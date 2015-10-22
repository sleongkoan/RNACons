package util;


import java.util.ArrayList;

/**
 * Node representation used for most trees in here
 */
public class Node<T> {

    // fields
    private ArrayList<Node<T>> children;
    private Node<T> parent;
    private T label;
    private int index;


    /**
     * Constructor
     * @param parent parent of the node, can be null
     * @param label  label of the node, a char
     */
    public Node(Node parent, T label) {
        // constructor
        if (parent != null) {
            parent.getChildren().add(this);
        }
        this.parent = parent;
        this.label = label;
        index = -1;
        children = new ArrayList<>();
    }


    public ArrayList<Node<T>> getChildren() {
        return children;
    }

    public Node<T> getParent() {
        return parent;
    }

    public T getLabel() {
        return label;
    }

    public int getIndex() {
        return index;
    }

    public void setChildren(ArrayList<Node<T>> children) {
        this.children = children;
    }

    public void setParent(Node<T> parent) {
        this.parent = parent;
    }

    public void setLabel(T label) {
        this.label = label;
    }

    public void setIndex(int index) {
        this.index = index;
    }
}


