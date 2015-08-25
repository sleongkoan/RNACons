#! /usr/bin/env python
"""save a consensus into heatmap + dendrogram form (with seaborn)"""

import argparse
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import scipy.cluster
import seaborn
from Util import *

seaborn.set(font="monospace")




class CustomClusterGrid(seaborn.matrix.Grid):
    def __init__(self, data, pivot_kws=None, figsize=None, row_colors=None, col_colors=None):
        """Grid object for organizing clustered heatmap input on to axes"""

        if isinstance(data, pd.DataFrame):
            self.data2d = data
        else:
            self.data2d = pd.DataFrame(data)

        if figsize is None:
            width, height = 10, 10
            figsize = (width, height)
        self.fig = plt.figure(figsize=figsize)

        if row_colors is not None:
            row_colors = _convert_colors(row_colors)
        self.row_colors = row_colors
        if col_colors is not None:
            col_colors = _convert_colors(col_colors)
        self.col_colors = col_colors

        width_ratios = self.dim_ratios(self.row_colors,
                                       figsize=figsize,
                                       axis=1)

        height_ratios = self.dim_ratios(self.col_colors,
                                        figsize=figsize,
                                        axis=0)
        nrows = 3 if self.col_colors is None else 4
        ncols = 3 if self.row_colors is None else 4

        self.gs = matplotlib.gridspec.GridSpec(nrows, ncols, wspace=0.01, hspace=0.01,
                                               width_ratios=width_ratios,
                                               height_ratios=height_ratios)

        self.ax_row_dendrogram = self.fig.add_subplot(self.gs[nrows - 1, 0:2],
                                                      axisbg="white")
        self.ax_col_dendrogram = self.fig.add_subplot(self.gs[0:2, ncols - 1],
                                                      axisbg="white")

        self.ax_row_colors = None
        self.ax_col_colors = None

        if self.row_colors is not None:
            self.ax_row_colors = self.fig.add_subplot(
                self.gs[nrows - 1, ncols - 2])
        if self.col_colors is not None:
            self.ax_col_colors = self.fig.add_subplot(
                self.gs[nrows - 2, ncols - 1])

        self.ax_heatmap = self.fig.add_subplot(self.gs[nrows - 1, ncols - 1])


        self.dendrogram_row = None
        self.dendrogram_col = None


    def dim_ratios(self, side_colors, axis, figsize, side_colors_ratio=0.05):
        figdim = figsize[axis]
        # Get resizing proportion of this figure for the dendrogram and
        # colorbar, so only the heatmap gets bigger but the dendrogram stays
        # the same size.
        dendrogram = min(2. / figdim, .2)

        # add the colorbar
        colorbar_width = .8 * dendrogram
        colorbar_height = .2 * dendrogram
        if axis == 0:
            ratios = [colorbar_width, colorbar_height]
        else:
            ratios = [colorbar_height, colorbar_width]

        if side_colors is not None:
            # Add room for the colors
            ratios += [side_colors_ratio]

        # Add the ratio for the heatmap itself
        ratios += [.8]

        return ratios

    @staticmethod
    def color_list_to_matrix_and_cmap(colors, ind, axis=0):
        # check for nested lists/color palettes.
        # Will fail if matplotlib color is list not tuple
        if any(issubclass(type(x), list) for x in colors):
            all_colors = set(itertools.chain(*colors))
            n = len(colors)
            m = len(colors[0])
        else:
            all_colors = set(colors)
            n = 1
            m = len(colors)
            colors = [colors]
        color_to_value = dict((col, i) for i, col in enumerate(all_colors))

        matrix = np.array([color_to_value[c]
                           for color in colors for c in color])

        shape = (n, m)
        matrix = matrix.reshape(shape)
        matrix = matrix[:, ind]
        if axis == 0:
            # row-side:
            matrix = matrix.T

        cmap = mpl.colors.ListedColormap(all_colors)
        return matrix, cmap

    def savefig(self, *args, **kwargs):
        if 'bbox_inches' not in kwargs:
            kwargs['bbox_inches'] = 'tight'
        self.fig.savefig(*args, **kwargs)

    def plot_dendrograms(self, row_cluster, col_cluster, metric, method,
                         row_linkage, col_linkage):
        # Plot the row dendrogram
        if row_cluster:
            self.dendrogram_row = seaborn.matrix.dendrogram(
                self.data2d, metric=metric, method=method, label=False, axis=0,
                ax=self.ax_row_dendrogram, rotate=True, linkage=row_linkage)
        else:
            self.ax_row_dendrogram.set_xticks([])
            self.ax_row_dendrogram.set_yticks([])
        # PLot the column dendrogram
        self.ax_col_dendrogram.set_xticks([])
        self.ax_col_dendrogram.set_yticks([])

    def plot_colors(self, xind, yind, **kws):
        # Remove any custom colormap and centering
        kws = kws.copy()
        kws.pop('cmap', None)
        kws.pop('center', None)
        kws.pop('vmin', None)
        kws.pop('vmax', None)
        kws.pop('xticklabels', None)
        kws.pop('yticklabels', None)
        if self.row_colors is not None:
            matrix, cmap = self.color_list_to_matrix_and_cmap(
                self.row_colors, yind, axis=0)
            seaborn.heatmap(matrix, cmap=cmap, cbar=False, ax=self.ax_row_colors,
                    xticklabels=False, yticklabels=False,
                    **kws)
        if self.col_colors is not None:
            matrix, cmap = self.color_list_to_matrix_and_cmap(
                self.col_colors, xind, axis=1)
            seaborn.heatmap(matrix, cmap=cmap, cbar=False, ax=self.ax_col_colors,
                    xticklabels=False, yticklabels=False,
                    **kws)

    def plot_matrix(self, colorbar_kws, xind, yind, **kws):
        self.data2d = self.data2d.iloc[yind, xind]

        seaborn.heatmap(self.data2d, ax=self.ax_heatmap, 
                cbar_kws=colorbar_kws, **kws)
        self.ax_heatmap.yaxis.set_ticks_position('right')
        self.ax_heatmap.yaxis.set_label_position('right')

    def plot(self, metric, method, colorbar_kws, row_cluster, col_cluster,
             row_linkage, col_linkage, **kws):
        colorbar_kws = {} if colorbar_kws is None else colorbar_kws
        self.plot_dendrograms(row_cluster, col_cluster, metric, method,
                              row_linkage=row_linkage, col_linkage=col_linkage)
        try:
            xind = self.dendrogram_col.reordered_ind
        except AttributeError:
            xind = np.arange(self.data2d.shape[1])
        try:
            yind = self.dendrogram_row.reordered_ind
        except AttributeError:
            yind = np.arange(self.data2d.shape[0])

        self.plot_colors(xind, yind, **kws)
        self.plot_matrix(colorbar_kws, xind, yind, **kws)
        return self


def clustermap(data, method='average', metric='euclidean',
               figsize=None,
               row_cluster=True, col_cluster=True,
               row_linkage=None, col_linkage=None,
               row_colors=None, col_colors=None, **kwargs):

    plotter = CustomClusterGrid(data, figsize=figsize,
                                row_colors=row_colors,
                                col_colors=col_colors)

    return plotter.plot(metric=metric, method=method,
                        colorbar_kws=[], row_cluster=row_cluster,
                        col_cluster=col_cluster, row_linkage=row_linkage,
                        col_linkage=col_linkage, **kwargs)


def compute_weighted_distances(consensus,
                               shape_weight,
                               tree_weight,
                               string_weight=1.):
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
    return cache[2*len(Z)], Z


def len_digit(integer):
    """calculates the number of digits in an integer number"""
    assert isinstance(integer, int)
    return len(str(integer))


def create_figure(consensus,
                  distance_matrix,
                  cluster_method="average",
                  annotate=False,
                  figsize=(20, 10),
                  show_shape=False):
    """show a dendrogram with the distance matrix with objects on the y axis"""
    print_order, linkage = get_order_linkage(distance_matrix,
                                             cluster_method=cluster_method)

    # create the labels
    ylabels = []
    max_digit_length = max(map(lambda x: len_digit(x), print_order))
    for position in print_order:
        padding_length = max_digit_length - len_digit(position)
        label = "[{}] ".format(position)
        label += "".join([" " for _ in range(padding_length)])
        if show_shape == True:
            label += " {}  ".format(consensus.abstract_shapes[position][0])
        ylabels.append(label)
    longest_label = max(map(lambda x: len(x), ylabels))
    for index, position in enumerate(print_order):
        padding = "".join([" " for _ in range(longest_label - len(ylabels[index]))])
        ylabels[index] = ylabels[index] + padding
        ylabels[index] = ylabels[index] + consensus.subopts[position]
    return clustermap(distance_matrix,
                      row_linkage=linkage,
                      annot=annotate,
                      col_cluster=False,
                      yticklabels=ylabels,
                      cbar=False,
                      figsize=figsize)


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
    # create the figure
    FIGURE = create_figure(CONSENSUS, DISTANCE_MATRIX, cluster_method=ARGS.cluster_method,
                           annotate=ARGS.annotate, figsize=(ARGS.x_size, ARGS.y_size),
                           show_shape=ARGS.show_shape)

    # save the figure
    FIGURE.savefig(ARGS.output_path)

