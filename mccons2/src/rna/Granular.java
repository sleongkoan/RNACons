package rna;

import sun.rmi.transport.proxy.CGIHandler;
import util.Node;
import util.OrderedRootedTree;

import java.util.ArrayList;
import java.util.LinkedList;


/**
 * Lossy tree compression ((((..)))).(.) -> (())() if granularity == 2
 */
public class Granular extends OrderedRootedTree {
    private int granularity;

    public Granular(String stringRepresentation, int divider)
    {
        super(stringRepresentation, '(', ')', '?');
        assert(divider > 0);
        this.granularity = divider;

        // apply reduction
        applyGranularTransformation();

    }


    private void applyGranularTransformation() {
        LinkedList<Node<Character>> searchSpace = new LinkedList<>();
        ArrayList<Node<Character>> stemStarters = new ArrayList<>();
        for (Node<Character> node : getRoot().getChildren())
        {
            searchSpace.add(node);
        }

        // find the nodes who begin a stem
        while (searchSpace.size() > 0) {
            Node<Character> currentNode = searchSpace.poll();
            if (startsStem(currentNode, getRoot())) {

                stemStarters.add(currentNode);
            }
            for (Node<Character> children : currentNode.getChildren()) {
                searchSpace.add(children);
            }
        }

        // replace old stems by new stems of specified size based on the previous one's size
        // (that's the granular transformation)
        for (Node node : stemStarters)
        {
            // size of the new stem
            int newLength = (int) Math.ceil((double) stemLength(node) / (double) granularity);
            ArrayList<Node<Character>> lastChildren = lastNodeOfStem(node).getChildren();

            Node<Character> position = node;
            for (int i = 1; i < newLength; ++i)
            {
                position = position.getChildren().get(0);
            }

            Node child;
            while(position.getChildren() != lastChildren)
            {
                // remove unwanted nodes from the stem
                child = position.getChildren().get(0);
                // remove parent pointer
                child.setParent(null);
                // remove child pointer and link grand-child
                position.setChildren(child.getChildren());
                child.setChildren(null);

            }
        }
    }


    /**
     * verifies if a node starts a stem. to do so it must either have a single children or none
     * and it must have a parent that is a junction or no parent at all
     * @param node position in the tree
     * @return if the node starts a stem or not
     */
    private boolean startsStem(Node node, Node artificialRoot)
    {
        boolean property1 = (node.getChildren().size() == 1 || node.getChildren().size() ==0);
        boolean property2 = (node.getParent() == artificialRoot || node.getParent().getChildren().size() > 1);
        return (property1 && property2);
    }


    private int stemLength(Node<Character> position)
    {
        int stemLength = 1;
        while(position.getChildren().size() == 1)
        {
            stemLength +=1;
            position = position.getChildren().get(0);
        }
        return stemLength;
    }

    private Node lastNodeOfStem(Node<Character> node)
    {
        Node<Character> position = node;
        while (position.getChildren().size() == 1)
        {
            position = position.getChildren().get(0);
        }
        return position;
    }
}
