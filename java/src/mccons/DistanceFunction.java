package mccons;

public interface DistanceFunction<T1, T2, T3> {

    T3 distance(T1 first, T2 second);
}


final class equalCharDistance implements DistanceFunction<Character, Character, Integer> {

    public Integer distance(Character first, Character second) {
        if (first.compareTo(second) == 0) {
            return 0;
        } else {
            return 2;
        }
    }
}


final class StringEditDistance implements DistanceFunction<String, String, Double> {

    public Double distance(String string1, String string2) {
        return new Double(string_edit_distance(string1, string2));

    }

    private int string_edit_distance(String string1,
                                     String string2) {
        // string edit distance
        int length1 = string1.length();
        int length2 = string2.length();
        char[] chars1 = string1.toCharArray();
        char[] chars2 = string2.toCharArray();
        int[][] stringDist = new int[length1 + 1][length2 + 1];

        // fill string1 column
        for (int i = 0; i < length1 + 1; ++i) {
            stringDist[i][0] = i;
        }

        // fill string2 row
        for (int j = 0; j < length2 + 1; ++j) {
            stringDist[0][j] = j;
        }

        int minDist, dist;
        // fill the whole table
        for (int i = 0; i < length1; ++i) {
            for (int j = 0; j < length2; ++j) {
                if (chars1[i] == chars2[j]) {
                    stringDist[i + 1][j + 1] = stringDist[i][j];
                } else {
                    minDist = stringDist[i][j + 1];
                    dist = stringDist[i + 1][j];
                    if (dist < minDist)
                        minDist = dist;
                    dist = stringDist[i][j];
                    if (dist < minDist)
                        minDist = dist;

                    stringDist[i + 1][j + 1] = minDist + 1;
                }
            }
        }
        return stringDist[length1][length2];
    }
}


final class TreeEditDistance implements DistanceFunction<OrderedRootedLabeledTree, OrderedRootedLabeledTree, Double> {


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
                new unitCostChar(),
                new unitCostChar(),
                new equalCharDistance());

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
