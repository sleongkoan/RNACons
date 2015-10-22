package cluster;

import ch.usi.inf.sape.hac.HierarchicalAgglomerativeClusterer;

import ch.usi.inf.sape.hac.agglomeration.AgglomerationMethod;
import ch.usi.inf.sape.hac.agglomeration.CompleteLinkage;
import ch.usi.inf.sape.hac.visualization.Dendrogram;
import ch.usi.inf.sape.hac.visualization.DendrogramBuilder;
import distances.DistanceFunction;


import java.util.ArrayList;

public final class Clusterer {

    public static <T> double[][] calculateDistanceMatrix(ArrayList<T> elements, DistanceFunction<T, T> distanceFunction)
    {
        double[][] distanceMatrix = new double[elements.size()][elements.size()];
        for (int i = 0; i != elements.size(); ++i)
        {
            for (int j = 0; j != elements.size(); ++j)
            {
                distanceMatrix[i][j] = distanceFunction.get(elements.get(i), elements.get(j));
            }
        }
        return  distanceMatrix;
    }


    public static <T> Dendrogram convertToDendrogram(ArrayList<T> consensus, DistanceFunction<T, T> distanceFunction)
    {
        AgglomerationMethod agglomeration = new CompleteLinkage();
        DendrogramBuilder builder = new DendrogramBuilder(consensus.size());

        HierarchicalAgglomerativeClusterer<T> clusterer = new HierarchicalAgglomerativeClusterer<>(calculateDistanceMatrix(consensus, distanceFunction), agglomeration);
        clusterer.cluster(builder);
        return builder.getDendrogram();

    }

}
