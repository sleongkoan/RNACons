#! /usr/bin/env python
"""save a consensus into heatmap + dendrogram form (with seaborn)"""

import argparse
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import scipy.cluster
import seaborn as sns
from Util import *

sns.set(font="monospace")
sns.set_style('white')


def compute_weighted_distances(consensus,
                               shape_weight,
                               tree_weight,
                               string_weight):
    """converts the multiple distances used into a single distance
       matrix by a simple weighted sum (many possibilities)"""

    # first, we'd like to establish a distance between shapes
    num_elems = len(consensus.subopts)
    shape_dists = np.zeros((num_elems, num_elems))

    # calculate the unit tree edit distance on the shapes
    for index_1 in range(num_elems - 1):
        (level_5_1, _, _) = consensus.abstract_shapes[index_1]
        dot_bracket_1 = level_5_1.replace("[", "(")
        dot_bracket_1 = dot_bracket_1.replace("]", ")")
        tree_1 = dot_bracket_to_tree(dot_bracket_1)
        for index_2 in range(index_1 + 1, num_elems):
            (level_5_2, _, _) = consensus.abstract_shapes[index_2]
            dot_bracket_2 = level_5_2.replace("[", "(")
            dot_bracket_2 = dot_bracket_2.replace("]", ")")
            tree_2 = dot_bracket_to_tree(dot_bracket_2)
            # assign it to the matrix
            dist = unlabeled_distance(tree_1, tree_2)
            shape_dists[index_1][index_2] = dist
            shape_dists[index_2][index_1] = dist

    # add the 3 matrices with proper weights
    new_matrix = np.zeros((num_elems, num_elems))
    for index_1 in range(num_elems):
        for index_2 in range(num_elems):
            shape_score = shape_weight * shape_dists[index_1][index_2]
            tree_score = tree_weight * consensus.tree_dists[index_1][index_2]
            string_score = (string_weight *
                            consensus.string_dists[index_1][index_2])
            total_score = tree_score + shape_score + string_score
            new_matrix[index_1][index_2] = total_score
    return new_matrix


def get_dendrogram(Z, labels):
    """returns the order in which the consensus should be presented"""
    # perform the hierarchical clustering
    return scipy.cluster.hierarchy.dendrogram(Z,  orientation="right", labels=labels, leaf_font_size=8)


def get_order_linkage(distance_matrix, cluster_method="average"):
    """returns the order in which the consensus should be presented"""
    # perform the hierarchical clustering
    mat_ut = distance_matrix[np.triu_indices(distance_matrix.shape[0], 1)]
    Z = scipy.cluster.hierarchy.linkage(mat_ut, method=cluster_method)

    # convert the linkage information to a dendogram ordering
    # http://stackoverflow.com/questions/12572436/calculate-ordering-of-dendrogram-leaves
    n = len(Z) + 1
    cache = dict()
    for k in range(len(Z)):
        c1, c2 = int(Z[k][0]), int(Z[k][1])
        c1 = [c1] if c1 < n else cache.pop(c1)
        c2 = [c2] if c2 < n else cache.pop(c2)
        cache[n+k] = c1 + c2
    return (cache[2*len(Z)], Z)

#def len_digit(integer):
    #"""calculates the number of digits in an integer number"""
    #assert isinstance(integer, int)
    #return len(str(integer))


#def create_figure(consensus,
                  #distance_matrix,
                  #cluster_method="average",
                  #annotate=False,
                  #figsize=(20, 10),
                  #show_shape=False):
    #"""show a dendrogram with the distance matrix with objects on the y axis"""
    #print_order, linkage = get_dendrogram(distance_matrix,
                                             #cluster_method=cluster_method)

    ## create the labels
    #ylabels = []
    #max_digit_length = max(map(lambda x: len_digit(x), print_order))
    #for position in print_order:
        #padding_length = max_digit_length - len_digit(position)
        #label = "[{}] ".format(position)
        #label += "".join([" " for _ in range(padding_length)])
        #if show_shape == True:
            #label += " {}  ".format(consensus.abstract_shapes[position][0])
        #ylabels.append(label)
    #longest_label = max(map(lambda x: len(x), ylabels))
    #for index, position in enumerate(print_order):
        #padding = "".join([" " for _ in range(longest_label - len(ylabels[index]))])
        #ylabels[index] = ylabels[index] + padding
        #ylabels[index] = ylabels[index] + consensus.subopts[position]
    #return clustermap(distance_matrix,
                      #row_linkage=linkage,
                      #annot=annotate,
                      #yticklabels=ylabels,
                      #cbar=False,
                      #figsize=figsize)


if __name__ == '__main__':
    # create the command line parser
    PARSER = argparse.ArgumentParser("displays a dendrogram and an "\
                                     "heatmap of a consensus")

    # I/O PARAMETERS
    PARSER.add_argument('-i', action="store", required=True,
                        type=str, dest="consensus_file_path",
                        help="path of the consensus file returned by MC-Cons")

    PARSER.add_argument('-o', action="store", required=True,
                        type=str, dest="output_path",
                        help="output file path (also specifies the graphic format)")

    # CONSENSUS INDEX (if there are more than one)
    PARSER.add_argument("--index", action="store", default=0,
                        type=int, dest="consensus_index",
                        help="index of the consensus to display")

    # CLUSTERING PARAMETERS
    PARSER.add_argument('--shapew', action="store", required=False,
                        type=float, dest="shape_weight", default=100.,
                        help="weight of the abstract shapes in the weighted sum")
    PARSER.add_argument('--treew', action="store", required=False,
                        type=float, dest="tree_weight", default=10.,
                        help="weight of the tree distances in the weighted sum")
    PARSER.add_argument('--stringw', action="store", required=False,
                        type=float, dest="string_weight", default=1.,
                        help="weight of the string edit distances in the weighted sum")
    PARSER.add_argument('--cluster', action="store", required=False,
                        type=str, dest="cluster_method", default="average",
                        choices=["single", "complete", "average", "weighted"],
                        help="choose the clustering method")

    # FIGURE PARAMETERS
    PARSER.add_argument("--xsize", action="store", default=10,
                        type=int, dest="x_size",
                        help="size of the x axis")
    PARSER.add_argument("--ysize", action="store", default=10,
                        type=int, dest="y_size",
                        help="size of the y axis")
    PARSER.add_argument("--annotate", action="store_true",
                        default=False, help="Display the weights in the distance matrix")
    PARSER.add_argument("--showshape", dest="show_shape", default=False,
                        action="store_true", required=False, help="Show the level 5 shape along the dot bracket")

    # parse the arguments
    ARGS = PARSER.parse_args()
    assert ARGS.shape_weight  >= 0.
    assert ARGS.tree_weight   >= 0.
    assert ARGS.string_weight >= 0.
    assert ARGS.x_size >= 0.
    assert ARGS.y_size >= 0.

    # aquire the consensus to display
    CONSENSUS = read_consensus_file(ARGS.consensus_file_path, ARGS.consensus_index)

    # calculate the weighted distance matrix
    DISTANCE_MATRIX = compute_weighted_distances(CONSENSUS,
                                                 ARGS.shape_weight,
                                                 ARGS.tree_weight,
                                                 ARGS.string_weight)


    FIGURE = plt.figure(figsize=(ARGS.x_size, ARGS.y_size))
    LABELS = [k for k in range(len(CONSENSUS.subopts))]
    ORDER, LINKAGE = get_order_linkage(DISTANCE_MATRIX)
    for i in ORDER:
        print CONSENSUS.subopts[i]
    DENDROGRAM = get_dendrogram(LINKAGE, LABELS)
  #  plt.xticks(rotation=90)
    no_spine = {'left': True, 'bottom': True, 'right': True, 'top': True}
    sns.despine(**no_spine);
    plt.savefig(ARGS.output_path)

    # save the figure
    FIGURE.savefig(ARGS.output_path)

