
/*
package mccons;

import mccons.OrderedRootedLabeledTree;
import mccons.RNAshapes;
import mccons.TreeEditDistance;

public class Main {

    public static void main(String[] args) {

        String db1 = "(((...)).(.))";
        String db2 = "(.(((..)).(.)..((())).))";
        OrderedRootedLabeledTree t1 = new OrderedRootedLabeledTree(db1, '(', ')', '.');
        OrderedRootedLabeledTree t2 = new OrderedRootedLabeledTree(db2, '(', ')', '.');


        // tree building
        /*
        System.out.println("tree building tests");
        String db3 = "(((())()((()))))";
        mccons.OrderedRootedLabeledTree t3 = new mccons.OrderedRootedLabeledTree(db3, '(', ')', '.');
        int[] kr = t3.getKeyroots();
        System.out.println(System.lineSeparator() + "keyroots");
        for (int k : kr)
        {
            System.out.print(k + " ");
        }
        System.out.println(System.lineSeparator() + "leftmost descendants");
        int[] lmds = t3.getLeftmostDescendants();
        for (int l : lmds)
        {
            System.out.print(l + " ");
        }

        System.out.println(System.lineSeparator() + "printing trees");
        System.out.println(db1 + " == " + t1.toString());
        System.out.println(db2 + " == " + t2.toString());

        // mccons.RNAshapes
        System.out.println(System.lineSeparator() + "mccons.RNAshapes");
        System.out.println(db2);
        System.out.println(RNAshapes.RNAshapes(db2, 1));
        System.out.println(RNAshapes.RNAshapes(db2, 3));
        System.out.println(RNAshapes.RNAshapes(db2, 5));

        // Tree distance
        String db3 = "(((...)).(.))".replace(".", "");
        OrderedRootedLabeledTree t3 = new OrderedRootedLabeledTree(db3, '(', ')', '.');

        TreeEditDistance t = new TreeEditDistance();

        System.out.println(t.distance(t1, t3));
        //System.out.println(new mccons.StringEditDistance().distance(db1, db2));
    }
}
*/