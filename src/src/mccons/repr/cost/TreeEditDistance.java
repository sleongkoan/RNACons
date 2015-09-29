package mccons.repr.cost;

import mccons.repr.*;

public class TreeEditDistance implements DistanceFunction<OrderedRootedLabeledTree, OrderedRootedLabeledTree, Double> {


    private CostFunction<Character, Double> deletionCost, insertionCost;
    private DistanceFunction<Character, Character, Double> substitutionCost;


    /**
     * constructor
     * @param deletionCost_ scoring function for deletions
     * @param insertionCost_ scoring function for insertions
     * @param substitutionCost_ scoring function for label replacement (to -> from)
     */
    public TreeEditDistance(CostFunction<Character, Double> deletionCost_,
                            CostFunction<Character, Double> insertionCost_,
                            DistanceFunction<Character, Character, Double> substitutionCost_)
    {
        deletionCost = deletionCost_;
        insertionCost = insertionCost_;
        substitutionCost = substitutionCost_;
    }


    /**
     * calculate the tree edit cost for two subtrees (helper)
     * @param tree1 ...
     * @param tree2 ...
     * @param keyRoot1 postorder index of the keyroot, first tree
     * @param keyRoot2 postorder index of the keyroot, second tree
     * @param dynamicProgrammingTable dynamic programming table to modify (tree cost)
     */
    public void treeToTreeDistance(OrderedRootedLabeledTree tree1,
                                   OrderedRootedLabeledTree tree2,
                                   int keyRoot1, int keyRoot2,
                                   double[][] dynamicProgrammingTable)
    {
        int[] FirstLMDS = tree1.getLeftmostDescendants();
        int[] SecondLMDS = tree2.getLeftmostDescendants();
        int p, q;

        int m = keyRoot1 - FirstLMDS[keyRoot1] + 2;
        int n = keyRoot2 - SecondLMDS[keyRoot2] + 2;

        // create a m x n matrix of zeros
        double[][] forestDistance = new double[m][n];

        // figure out the offset
        int iOffset = FirstLMDS[keyRoot1] - 1;
        int jOffset = SecondLMDS[keyRoot2] - 1;

        // some data we'll use along the way
        Character label1, label2;
        double insert, del, sub;


        // ====================DYNAMIC PROGRAMMING TABLE FILLING===================
        // fill deletions (tree1 row) and insertions (tree1 column)
        for (int x = 1; x != m; ++x) {
            forestDistance[x][0] = forestDistance[x - 1][0] + 1;
        }
        for (int y = 1; y != n; ++y) {
            forestDistance[0][y] = forestDistance[0][y - 1] + 1;
        }


        // fill the rest of the matrix
        for (int x = 1; x != m; ++x) {
            for (int y = 1; y != n; ++y) {
                // some situation independent data
                label1 = tree1.getPostOrderLabels()[(x + iOffset)];
                label2 = tree2.getPostOrderLabels()[(y + jOffset)];

                del = deletionCost.get(label1);
                insert = insertionCost.get(label2);

                // case 1
                // x is an ancestor of i and y is an ancestor of j
                if ((FirstLMDS[keyRoot1] == FirstLMDS[x + iOffset]) &&
                        (SecondLMDS[keyRoot2] == SecondLMDS[y + jOffset])) {
                    sub = substitutionCost.get(label1, label2);
                    forestDistance[x][y] = Math.min(Math.min(
                                    // deletion
                                    (forestDistance[x - 1][y] + del),
                                    // insertion
                                    (forestDistance[x][y - 1] + insert)),
                            // substitution
                            (forestDistance[x - 1][y - 1] + sub)
                    );

                    dynamicProgrammingTable[x + iOffset][y + jOffset] = forestDistance[x][y];
                }
                // case 2
                else {
                    p = FirstLMDS[x + iOffset] - 1 - iOffset;
                    q = SecondLMDS[y + jOffset] - 1 - jOffset;
                    forestDistance[x][y] = Math.min(Math.min(
                                    // deletion
                                    (forestDistance[x - 1][y] + del),
                                    // insertion
                                    (forestDistance[x][y - 1] + insert)),
                            // substitution
                            (forestDistance[p][q] + dynamicProgrammingTable[x + iOffset][y + jOffset])
                    );
                }
            }
        }
    }


    /**
     * return the dynamic programming matrix for the tree edit get between two whole trees
     * @param tree1 first whole tree to compare
     * @param tree2 second whole tree to compare
     * @return dynamic programming table of get
     */
    public double[][] treeEditDistance(OrderedRootedLabeledTree tree1,
                                       OrderedRootedLabeledTree tree2)
    {
        int sizeTree1 = tree1.getPostOrderLabels().length;
        int sizeTree2 = tree2.getPostOrderLabels().length;

        // create the matrix holding tree cost
        double[][] treeDistances = new double[sizeTree1][sizeTree2];


        int[] keyRoots1 = tree1.getKeyRoots();
        int[] keyRoots2 = tree2.getKeyRoots();
        int keyRoot1, keyRoot2;
        for (int i = 0; i != keyRoots1.length; ++i) {
            keyRoot1 = keyRoots1[i];
            for (int j = 0; j != keyRoots2.length; ++j) {
                keyRoot2 = keyRoots2[j];
                treeToTreeDistance(tree1, tree2,
                        keyRoot1, keyRoot2,
                        treeDistances);
            }
        }
        return treeDistances;
    }


    /**
     * interface method, based on internal methods
     * @param tree1 first whole tree
     * @param tree2 second whole tree
     * @return get between the two
     */
    public Double get(OrderedRootedLabeledTree tree1,
                      OrderedRootedLabeledTree tree2) {
        double[][] dynamicProgrammingTable = treeEditDistance(tree1, tree2);
        return dynamicProgrammingTable[dynamicProgrammingTable.length - 1][dynamicProgrammingTable[0].length - 1];
    }


}
