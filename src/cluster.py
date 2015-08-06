#! /usr/bin/env python
"""Clustering functions used when pretty printing consensus"""

# some clustering utilities
import scipy.cluster
import seaborn


def get_dist_matrix(sol, problem):
    """creates a new distance matrix with only solutions"""
    matrix_size = len(sol.genes)
    distance_matrix = np.zeros((matrix_size, matrix_size))
    for index1, gene1 in enumerate(sol.genes):
        for index2, gene2 in enumerate(sol.genes):
            if index2 > index1:
                dist = problem.distance_matrix[gene1][gene2]
                distance_matrix[index1][index2] = dist
                distance_matrix[index2][index1] = dist
    return distance_matrix


def get_cluster_information(distance_matrix, cluster_method="average"):
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


def show_solution(solution, problem, cluster_method="average", show_objects=True):
    """show a dendrogram with the distance matrix"""
    distance_matrix = get_dist_matrix(solution, problem)
    print_order, linkage = get_cluster_information(distance_matrix, cluster_method=cluster_method)
    
    # get the objects
    if show_objects:
        ylabels = []
        max_num_digits = divmod(len(print_order), 10)[0]
        for position in print_order:
            gene = solution.genes[position]
            num_digits = divmod(position, 10)[0]
            padding = "".join([" " for _ in range(max_num_digits - num_digits)])
            ylabels.append("[{}] {} {}".format(position, padding, problem.objects[gene]))
        return clustermap(distance_matrix, row_linkage=linkage, annot=True, col_cluster=False, yticklabels=ylabels, cbar=False)
    else:
        return clustermap(distance_matrix, row_linkage=linkage, annot=True, col_cluster=False, cbar=False)



import pandas as pd


def _matrix_mask(data, mask):
    """Ensure that data and mask are compatabile and add missing values.

    Values will be plotted for cells where ``mask`` is ``False``.

    ``data`` is expected to be a DataFrame; ``mask`` can be an array or
    a DataFrame.

    """
    if mask is None:
        mask = np.zeros(data.shape, np.bool)

    if isinstance(mask, np.ndarray):
        # For array masks, ensure that shape matches data then convert
        if mask.shape != data.shape:
            raise ValueError("Mask must have the same shape as data.")

        mask = pd.DataFrame(mask,
                            index=data.index,
                            columns=data.columns,
                            dtype=np.bool)

    elif isinstance(mask, pd.DataFrame):
        # For DataFrame masks, ensure that semantic labels match data
        if not mask.index.equals(data.index) \
           and mask.columns.equals(data.columns):
            err = "Mask must have the same index and columns as data."
            raise ValueError(err)

    # Add any cells with missing data to the mask
    # This works around an issue where `plt.pcolormesh` doesn't represent
    # missing data properly
    mask = mask | pd.isnull(data)

    return mask

class CustomClusterGrid(seaborn.matrix.Grid):
    def __init__(self, data, pivot_kws=None, z_score=None, standard_scale=None,
                 figsize=None, row_colors=None, col_colors=None, mask=None):
        """Grid object for organizing clustered heatmap input on to axes"""

        if isinstance(data, pd.DataFrame):
            self.data = data
        else:
            self.data = pd.DataFrame(data)

        self.data2d = self.format_data(self.data, pivot_kws, z_score,
                                       standard_scale)

        self.mask = _matrix_mask(self.data2d, mask)

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

    def format_data(self, data, pivot_kws, z_score=None,
                    standard_scale=None):
        # Either the data is already in 2d matrix format, or need to do a pivot
        if pivot_kws is not None:
            data2d = data.pivot(**pivot_kws)
        else:
            data2d = data

        if z_score is not None and standard_scale is not None:
            raise ValueError(
                'Cannot perform both z-scoring and standard-scaling on data')

        if z_score is not None:
            data2d = self.z_score(data2d, z_score)
        if standard_scale is not None:
            data2d = self.standard_scale(data2d, standard_scale)
        return data2d

    @staticmethod
    def z_score(data2d, axis=1):
        if axis == 1:
            z_scored = data2d
        else:
            z_scored = data2d.T

        z_scored = (z_scored - z_scored.mean()) / z_scored.var()

        if axis == 1:
            return z_scored
        else:
            return z_scored.T

    @staticmethod
    def standard_scale(data2d, axis=1):
        # Normalize these values to range from 0 to 1
        if axis == 1:
            standardized = data2d
        else:
            standardized = data2d.T

        subtract = standardized.min()
        standardized = (standardized - subtract) / (
            standardized.max() - standardized.min())

        if axis == 1:
            return standardized
        else:
            return standardized.T

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
        if col_cluster:
            self.dendrogram_col = dendrogram(
                self.data2d, metric=metric, method=method, label=False,
                axis=1, ax=self.ax_col_dendrogram, linkage=col_linkage)
        else:
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
        self.mask = self.mask.iloc[yind, xind]
        seaborn.heatmap(self.data2d, ax=self.ax_heatmap, 
                cbar_kws=colorbar_kws, mask=self.mask, **kws)
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

def clustermap(data, pivot_kws=None, method='average', metric='euclidean',
               z_score=None, standard_scale=None, figsize=None, cbar_kws=None,
               row_cluster=True, col_cluster=True,
               row_linkage=None, col_linkage=None,
               row_colors=None, col_colors=None, mask=None, **kwargs):

    plotter = CustomClusterGrid(data, pivot_kws=pivot_kws, figsize=figsize,
                          row_colors=row_colors, col_colors=col_colors,
                          z_score=z_score, standard_scale=standard_scale,
                          mask=mask)

    return plotter.plot(metric=metric, method=method,
                        colorbar_kws=cbar_kws,
                        row_cluster=row_cluster, col_cluster=col_cluster,
                        row_linkage=row_linkage, col_linkage=col_linkage,
                        **kwargs)


# look at the first tree solution
#show_solution(tree_solutions[0], tree_problem, cluster_method="complete")

# look at the first dot bracket solution
#ex = show_solution(dot_bracket_solutions[0][0], dot_bracket_problems[0], cluster_method="complete")


#ex.savefig("../../article/figs/consensus.eps")
