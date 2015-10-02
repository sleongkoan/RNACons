package mccons.repr;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;


/**
 * Ordered labeled tree used to represent both RNA abstract shapes
 * and Vienna dot-bracket (planar ordered labeled tree)
 */

public class OrderedRootedLabeledTree {

    private char[] postOrderLabels;
    private int[] leftmostDescendants;
    private int[] keyRoots;

    private Node root;
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
        assert (isValidTree(stringRepresentation_, nestingSymbol_, closingSymbol_, addNodeSymbol_));

        // 1- assign the string representation and remember the symbols
        nestingSymbol = nestingSymbol_;
        closingSymbol = closingSymbol_;
        addNodeSymbol = addNodeSymbol_;

        // 2- create rooted tree
        root = new Node(null, nestingSymbol);
        Node position = root;

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

        // 6- fetch the keyRoots
        Hashtable<Integer, Integer> nodeToLeftmostDescendant = new Hashtable<>();
        Integer lmd;
        for (int node_index = postorderNodes.size() - 1; node_index != -1; --node_index) {
            lmd = leftmostDescendants[node_index];

            if (!(nodeToLeftmostDescendant.containsKey(lmd))) {
                nodeToLeftmostDescendant.put(lmd, node_index);
            }
        }

        keyRoots = new int[nodeToLeftmostDescendant.size()];
        Enumeration<Integer> keyrootsEnum = nodeToLeftmostDescendant.elements();
        int index = 0;
        while (keyrootsEnum.hasMoreElements()) {
            keyRoots[index] = keyrootsEnum.nextElement();
            index += 1;
        }
        java.util.Arrays.sort(keyRoots);
    }


    /**
     * copy constructor
     *
     * @param other Ordered Labeled Tree to copy
     */
    public OrderedRootedLabeledTree(OrderedRootedLabeledTree other) {
        this(other.toString(), other.getNestingSymbol(), other.getClosingSymbol(), other.getAddNodeSymbol());
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
     * @return the keyRoots of the tree
     */
    public int[] getKeyRoots() {
        return keyRoots;
    }


    private void print_helper(Node position, ArrayList<Character> symbolList)
    { //
        if (position.getLabel() == nestingSymbol)
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
        StringBuilder builder = new StringBuilder(symbolList.size());
        for (Character c  : symbolList)
        {
            builder.append(c);
        }
        return builder.toString();
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