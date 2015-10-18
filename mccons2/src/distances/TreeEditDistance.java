package distances;

import convenience.RTED;
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

    public Double get(String dotBracket1, String dotBracket2) {
        // add artificial root
        String tree1 = "(" + dotBracket1 + ")";
        String tree2 = "(" + dotBracket2 + ")";
        return RTED.computeDistance(Util.dotBracketToRTED(tree1), Util.dotBracketToRTED(tree2));
    }
}
