
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;


/**
 * Node representation used for most trees in here
 */
class Node {

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


/**
 * Ordered labeled tree used to represent both RNA abstract shapes
 * and Vienna dot-bracket (planar ordered labeled tree)
 */

public class OrderedRootedLabeledTree {

    private char[] postOrderLabels;
    private int[] leftmostDescendants;
    private int[] keyroots;

    private Node root;
    private String stringRepresentation;
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
    public OrderedRootedLabeledTree(String stringRepresentation_,
                                    char nestingSymbol_,
                                    char closingSymbol_,
                                    char addNodeSymbol_) {
        // main constructor
        // 1- assign the string representation and remember the symbols
        stringRepresentation = stringRepresentation_;
        nestingSymbol = nestingSymbol_;
        closingSymbol = closingSymbol_;
        addNodeSymbol = addNodeSymbol_;

        // 2- create rooted tree
        root = new Node(null, nestingSymbol);
        Node position = root;

        for (char c : stringRepresentation.toCharArray()) {
            if (c == nestingSymbol)       // create new node and position goes down
            {
                position = new Node(position, nestingSymbol);
            } else if (c == closingSymbol)  // position goes up
            {
                position = position.getParent();
            } else if (c == addNodeSymbol) // add unpaired node, position stays same
            {
                new Node(position, addNodeSymbol);
            } else {
                throw new IllegalArgumentException("Illegal symbol used in the string representation");
            }
        }

        // 3- get postorder pointers
        ArrayList<Node> postorderNodes = getPostOrderEnumeration(root);

        // 4- set the index and fetch labels
        postOrderLabels = new char[postorderNodes.size()];
        for (int index = 0; index != postorderNodes.size(); ++index) {
            postorderNodes.get(index).setIndex(index);
            postOrderLabels[index] = postorderNodes.get(index).getLabel();
        }

        // 5- fetch the leftmost descendents
        leftmostDescendants = new int[postorderNodes.size()];
        for (int index = 0; index != postorderNodes.size(); ++index) {
            leftmostDescendants[index] = postorderNodes.get(index).getLeftmostDescendantIndex();
        }

        // 6- fetch the keyroots
        Hashtable<Integer, Integer> nodeToLeftmostDescendant = new Hashtable<>();
        Integer lmd;
        for (int node_index = postorderNodes.size() - 1; node_index != -1; --node_index) {
            lmd = leftmostDescendants[node_index];

            if (!(nodeToLeftmostDescendant.containsValue(lmd))) {
                nodeToLeftmostDescendant.put(lmd, node_index);
            }
        }

        keyroots = new int[nodeToLeftmostDescendant.size()];
        Enumeration<Integer> keyrootsEnum = nodeToLeftmostDescendant.elements();
        int index = 0;
        while (keyrootsEnum.hasMoreElements()) {
            keyroots[index] = keyrootsEnum.nextElement();
            index += 1;
        }
        java.util.Arrays.sort(keyroots);
    }


    /**
     * copy constructor
     *
     * @param other Ordered Labeled Tree to copy
     */
    public OrderedRootedLabeledTree(OrderedRootedLabeledTree other) {
        this(other.getStringRepresentation(),
                other.getNestingSymbol(),
                other.getClosingSymbol(),
                other.getAddNodeSymbol());
    }


    /**
     * verifies that the arguments given to the constructor build a legal tree
     * @param stringRepresentation_ string representation, usually Vienna dot bracket
     * @param nestingSymbol_ '(' for dot bracket
     * @param closingSymbol_ ')' for dot bracket
     * @param addNodeSymbol_ '.' for dot bracket
     * @return true if a valid tree is build from the arguments
     */
    public static boolean isValidTree(String stringRepresentation_,
                                      char nestingSymbol_, char closingSymbol_, char addNodeSymbol_)
    {
        int counter = 0;
        char[] charRepresentation = stringRepresentation_.toCharArray();

        for (char c : charRepresentation)
        {
            if (c == nestingSymbol_)               // stack
            {
                counter += 1;
            }
            else if (c == closingSymbol_)          // unstack
            {
                counter -= 1;
            }
            else if (c != addNodeSymbol_)          // illegal character
            {
                return false;
            }
            if (counter < 0)            // left unbalanced
            {
                return false;
            }
        }
        if (counter != 0)               // right unbalanced
        {
            return false;
        }
        else                            // correct tree
        {
            return true;
        }
    }

    /**
     * recursive helper to go through the nodes and add them to array
     * during a postorder traversal
     *
     * @param node  current node in the traversal
     * @param array contains the nodes in postorder
     */
    private void postorder_helper(Node node, ArrayList<Node> array) {
        if (node != null) {
            for (Node n : node.getChildren()) {
                postorder_helper(n, array);
            }
            array.add(node);
        }
    }


    /**
     * get list of node pointers to the postorder enumeration
     *
     * @param root root node of the tree
     * @return an arraylist of the post-order traversal of the tree
     */
    private ArrayList<Node> getPostOrderEnumeration(Node root) {
        ArrayList<Node> postOrderEnum = new ArrayList<>();
        postorder_helper(root, postOrderEnum);
        return postOrderEnum;
    }

    public Node getRoot() {
        return root;
    }

    /**
     * getter
     *
     * @return labels in post-order traversal order
     */
    public char[] getPostOrderLabels() {
        return postOrderLabels;
    }

    /**
     * getter
     *
     * @return leftmost descendants for each of the nodes
     */
    public int[] getLeftmostDescendants() {
        return leftmostDescendants;
    }

    /**
     * getter
     *
     * @return the keyroots of the tree
     */
    public int[] getKeyroots() {
        return keyroots;
    }


    private void print_helper(Node position, ArrayList<Character> symbolList)
    { //
        if (position.getLabel() == addNodeSymbol)
        {
            symbolList.add(nestingSymbol);
            for (Node newPosition : position.getChildren())
            {
                print_helper(newPosition, symbolList);
            }
            symbolList.add(closingSymbol);
        }
        else if (position.getLabel() == addNodeSymbol)
        {
            symbolList.add(addNodeSymbol);
        }
    }


    public String toString()
    {
        ArrayList<Character> symbolList = new ArrayList<>();
        for (Node position : root.getChildren())
        {
            print_helper(position, symbolList);
        }
        return symbolList.toString();
    }



    /**
     * getter
     *
     * @return the string representation of the tree
     */
    public String getStringRepresentation() {
        return stringRepresentation;
    }

    public char getNestingSymbol() {
        return nestingSymbol;
    }

    public char getClosingSymbol() {
        return closingSymbol;
    }

    public char getAddNodeSymbol() {
        return addNodeSymbol;
    }
}
