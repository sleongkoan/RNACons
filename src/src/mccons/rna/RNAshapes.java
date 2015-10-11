package mccons.rna;

import java.util.ArrayDeque;
import java.util.ArrayList;


public final class RNAshapes {

    /**
     * remove the stems
     * @param node
     * @return
     */
    public static boolean remove_stem(Node node, char nestingSymbol)
    {   // to be used in a BFS traversal only
        if ( (node.getLabel() == nestingSymbol) && (node.getChildren().size() == 1) )
        {
            node.setChildren(node.getChildren().get(0).getChildren());
            return true;
        }
        else
        {
            return false;
        }
    }


    private static String charArrayListToString(ArrayList<Character> input)
    {
        StringBuilder builder = new StringBuilder(input.size());
        for (Character c  : input)
        {
            builder.append(c);
        }
        return builder.toString();
    }


    public static void remove_stems(Node tree, char nestingSymbol)
    {
        ArrayDeque<Node> Q = new ArrayDeque<>();
        Q.addLast(tree);
        Node current_node;

        boolean modified;
        while(!Q.isEmpty())
        {
            // Dequeue
            current_node = Q.pollFirst();

            // apply stem removing operation and check status
            modified = remove_stem(current_node, nestingSymbol);
            if (modified)
            {
                // requeue immediately
                Q.addFirst(current_node);
            }
            else if (current_node.getLabel() == nestingSymbol)
            {
                for (int i = 0; i != current_node.getChildren().size(); ++i)
                {
                    Q.addLast( current_node.getChildren().get(i));
                }
            }
        }
    }


    public static String preprocess(String dot_bracket)
    {
        ArrayList<Character> step1 = new ArrayList<>();
        char[] chars = dot_bracket.toCharArray();
        char lastChar = 'a';
        for (char currentChar : chars)
        {
            if ( ! ((currentChar =='.') && (lastChar == currentChar)))
            {
                // add it
                step1.add(currentChar);
            }
            lastChar = currentChar;
        }

        // (.) . ()
        ArrayList<Character> step2 = new ArrayList<>();
        step2.add(step1.get(0));

        for(int i = 1; i != step1.size()-1; ++i)
        {
            if (!( (step2.get(step2.size() - 1) == '(') && (step1.get(i) == '.') && (step1.get(i+1) == ')') ))
            {
                step2.add(step1.get(i));
            }
        }
        step2.add(step1.get(step1.size() - 1));

        return charArrayListToString(step2);
    }

    public static String dotBracketToAbstractShape(String dot_bracket, int level)
    {
        assert(level == 1 || level == 3 || level==5);

        // preprocess the dotbracket and convert it to tree representation
        String processed = preprocess(dot_bracket);

        OrderedRootedTree tree = new OrderedRootedTree(processed, '(', ')', '.');
        ArrayList<Node> subTrees = tree.getRoot().getChildren();



        //-------------------------------------------level 1
        // must remove nodes with single children (stems without branching)
        for (Node subRoot : subTrees)
        {
            remove_stems(subRoot, '(');
        }
        String level1 = tree.toString().replace("(", "[").replace(")", "]").replace(".", "_");
        if (level == 1)
        {
            return level1;
        }


        //-------------------------------------------level 3
        // we use String replacement here, its easier
        String level3 = level1.replace("_", "");
        if (level == 3)
        {
            return level3;
        }

        //-------------------------------------------level 5
        // same as for level 1 except it is applied on the level 3 String
        OrderedRootedTree tree2 = new OrderedRootedTree(level3, '[', ']', '_');

        ArrayList<Node> trees2 = tree2.getRoot().getChildren();
        for (Node n : trees2)
        {
            remove_stems(n, '[');
        }
        return tree2.toString();
    }
}
