package mccons.repr.distances;

import mccons.repr.*;

public final class TreeEditDistance implements DistanceFunction<OrderedRootedLabeledTree, OrderedRootedLabeledTree, Double> {


// ===============================TREE DISTANCES===============================


    public static void treeToTreeDistance(OrderedRootedLabeledTree tree1,
                            OrderedRootedLabeledTree tree2, // trees
                            int i, int j,             // keyroots indices
                            int[][] treedists,        // table to modify
                            CostFunction<Character, Integer> insertion_fun,  // insertion
                            CostFunction<Character, Integer> deletion_fun,   // deletion
                            DistanceFunction<Character, Character, Integer> substitution_fun) // substitution
    {  // calculate the tree edit distances for two subtrees (helper)

        // =============================INITIALIZATION=============================
        int[] FirstLMDS = tree1.getLeftmostDescendants();
        int[] SecondLMDS = tree2.getLeftmostDescendants();
        int p, q;

        int m = i - FirstLMDS[i] + 2;
        int n = j - SecondLMDS[j] + 2;

        // create a m x n matrix of zeros
        int[][] forestDistance = new int[m][n];

        // figure out the offset
        int iOffset = FirstLMDS[i] - 1;
        int jOffset = SecondLMDS[j] - 1;

        // some data we'll use along the way
        Character label1, label2;
        int insertion_cost, deletion_cost, substitution_cost;


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

                deletion_cost = deletion_fun.calculateCost(label1);
                insertion_cost = insertion_fun.calculateCost(label2);

                // case 1
                // x is an ancestor of i and y is an ancestor of j
                if ((FirstLMDS[i] == FirstLMDS[x + iOffset]) &&
                        (SecondLMDS[j] == SecondLMDS[y + jOffset])) {
                    substitution_cost = substitution_fun.distance(label1, label2);
                    forestDistance[x][y] = Math.min(Math.min(
                                    // deletion
                                    (forestDistance[x - 1][y] + deletion_cost),
                                    // insertion
                                    (forestDistance[x][y - 1] + insertion_cost)),
                            // substitution
                            (forestDistance[x - 1][y - 1] + substitution_cost)
                    );

                    treedists[x + iOffset][y + jOffset] = forestDistance[x][y];
                }
                // case 2
                else {
                    p = FirstLMDS[x + iOffset] - 1 - iOffset;
                    q = SecondLMDS[y + jOffset] - 1 - jOffset;
                    forestDistance[x][y] = Math.min(Math.min(
                                    // deletion
                                    (forestDistance[x - 1][y] + deletion_cost),
                                    // insertion
                                    (forestDistance[x][y - 1] + insertion_cost)),
                            // substitution
                            (forestDistance[p][q] + treedists[x + iOffset][y + jOffset])
                    );
                }
            }
        }
    }


    public static int[][] treeEditDistance(OrderedRootedLabeledTree tree1,
                             OrderedRootedLabeledTree tree2, // trees
                             CostFunction<Character, Integer> insertion_fun,  // insertion
                             CostFunction<Character, Integer> deletion_fun,   // deletion
                             DistanceFunction<Character, Character, Integer> substitution_fun) // substitution
    {  // return the dynamic programming matrix for the tree edit distance between two whole trees

        // =============================INITIALIZATION=============================

        int sizeTree1 = tree1.getPostOrderLabels().length;
        int sizeTree2 = tree2.getPostOrderLabels().length;

        // create the matrix holding tree distances
        int[][] tree_distances = new int[sizeTree1][sizeTree2];

        // =======================TREE DISTANCES CALCULATIONS======================
        int[] keyRoots1 = tree1.getKeyroots();
        int[] keyRoots2 = tree2.getKeyroots();
        int keyroot1, keyroot2;
        for (int i = 0; i != keyRoots1.length; ++i) {
            keyroot1 = keyRoots1[i];
            for (int j = 0; j != keyRoots2.length; ++j) {
                keyroot2 = keyRoots2[j];
                treeToTreeDistance(tree1, tree2,
                        keyroot1, keyroot2,
                        tree_distances,
                        insertion_fun,
                        deletion_fun,
                        substitution_fun);
            }
        }
        return tree_distances;
    }

    public Double distance(OrderedRootedLabeledTree tree1,
                           OrderedRootedLabeledTree tree2) {
        int[][] dynamicProgrammingTable = treeEditDistance(tree1, tree2,
                new UnitCostChar(),
                new UnitCostChar(),
                new EqualCharDistance());

        /*
        // print the dynamic programming table
        System.out.println("dynamic programming table result");
        for (int[] v : dynamicProgrammingTable) {
            for (int d : v) {
                System.out.print(d + " ");
            }
            System.out.println();
        }
        */
        return new Double(dynamicProgrammingTable[dynamicProgrammingTable.length - 1][dynamicProgrammingTable[0].length - 1]);
    }


}
