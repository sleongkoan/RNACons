package distances;

import distance.RTED_InfoTree_Opt;
import util.Util;
import util.LblTree;

public class TreeEditDistance implements DistanceFunction<String, String, Double>{

    double costIns, costDel, costRen;

    public TreeEditDistance(double costIns, double costDel, double costRen) {
        this.costIns = costIns;
        this.costDel = costDel;
        this.costRen = costRen;
    }

    public Double get(String tree1, String tree2) {
        RTED_InfoTree_Opt rted = new RTED_InfoTree_Opt(costIns, costDel, costRen);
        return rted.nonNormalizedTreeDist(LblTree.fromString(Util.dotBracketToRTED(tree1)), LblTree.fromString(Util.dotBracketToRTED(tree2)));
    }
}
