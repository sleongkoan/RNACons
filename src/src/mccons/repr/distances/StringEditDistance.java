package mccons.repr.distances;

public final class StringEditDistance implements DistanceFunction<String, String, Double> {

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
