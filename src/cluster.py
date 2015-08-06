"""simple pure python UPGMA implementation"""

from util import (Consensus, read_consensus_file,
                  unlabeled_distance, dot_bracket_to_tree)


def _compute_weighted_distances(consensus,
                                shape_weight=10000000.,
                                tree_weight=100000.,
                                string_weight=1.):
    """converts the multiple distances used into a single distance
       matrix by a simple weighted sum (many possibilities)"""

    # first, we'd like to establish a distance between shapes
    num_elems = len(consensus.subopts)
    shape_dists = [[0. for _ in range(num_elems)] for _ in range(num_elems)]
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
    new_matrix = [[0. for _ in range(num_elems)] for _ in range(num_elems)]
    for index_1 in range(num_elems):
        for index_2 in range(num_elems):
            shape_score = shape_weight * shape_dists[index_1][index_2]
            tree_score = tree_weight * consensus.tree_dists[index_1][index_2]
            string_score = (string_weight *
                            consensus.string_dists[index_1][index_2])
            total_score = tree_score + shape_score + string_score
            new_matrix[index_1][index_2] = total_score
    return new_matrix


def _matrix_to_dict(matrix):
    """convert a matrix to a dict, keeping only lower triangular part"""
    distance_dict = dict()
    for index_1 in range(len(matrix)):
        for index_2 in range(index_1 + 1, len(matrix)):
            distance_dict[(index_1, index_2)] = matrix[index_1][index_2]
    return distance_dict


def _dict_argmin(distance_dict):
    """return the keys of the minimal value"""
    assert isinstance(distance_dict, dict)
    best_value = float('inf')
    best_key = None
    for key, value in distance_dict.iteritems():
        if value < best_value:
            best_key = key
            best_value = value
    return best_key, best_value


def _flatten_nested_tuple(nested_tuple):
    """convert a nested tuple of integer indices into a flat list"""
    flat_list = []
    tuples_list = [nested_tuple]
    while tuples_list:
        # decompose the tuple into its constituents
        current_tuple = tuples_list.pop()
        if isinstance(current_tuple, tuple):
            for elem in current_tuple:
                if isinstance(elem, tuple):
                    tuples_list.append(elem)
                else:
                    flat_list.append(elem)
        else:
            flat_list.append(current_tuple)
    return flat_list


def _average_distance(distance_matrix, group1, group2):
    """calculate the arithmetic mean of the distances between
    all elems of the two groups"""
    num_compared = len(group1) * len(group2)
    total = 0.
    for index1 in group1:
        for index2 in group2:
            total += distance_matrix[index1][index2]
    return total / num_compared


def _update_distance_dict(distance_dict, distance_matrix, key1, key2):
    """updates the matrix dictionary by recalculating distances"""
    # remove useless keys
    useless_keys = set()
    remaining_keys = set()
    key = (key1, key2)
    for part1, part2 in distance_dict.iterkeys():
        if part1 in key:
            useless_keys.add((part1, part2))
        else:
            remaining_keys.add(part1)
        if part2 in key:
            useless_keys.add((part1, part2))
        else:
            remaining_keys.add(part2)
    for key in useless_keys:
        del distance_dict[key]

    # create the new key
    new_key = (key1, key2)
    elems1 = _flatten_nested_tuple(new_key)

    # create a dict mapping keys to their elements
    keys_to_flat = dict()
    for key in remaining_keys:
        keys_to_flat[key] = _flatten_nested_tuple(key)

    # recalculate average distance between old keys and new key
    for key, elems2 in keys_to_flat.iteritems():
        distance_dict[(key, new_key)] = _average_distance(distance_matrix,
                                                          elems1,
                                                          elems2)
    return distance_dict


def linkage_upgma(distance_matrix):
    """compute the Unweighted Pair Group Method with Arithmetic Mean linkage"""
    # make sure that the matrix is at least symmetric
    for i in range(len(distance_matrix)):
        for j in range(len(distance_matrix)):
            assert distance_matrix[i][j] == distance_matrix[j][i]
    # make sure that the distance(i, i) = 0
    for i in range(len(distance_matrix)):
        assert distance_matrix[i][i] == 0

    # convert to a dictionary
    distance_dict = _matrix_to_dict(distance_matrix)

    # start the UPGMA algorithm
    final_cluster = ()
    fusions = []
    while True:
        print "\n"
        for (k1, k2), v in distance_dict.iteritems():
            print "{}, {} : {}".format(k1, k2, v)
        print "\n"
        if len(distance_dict) <= 2:  # last choice
            final_cluster, cost = _dict_argmin(distance_dict)
            fusions.append((final_cluster[0], final_cluster[1], cost))
            break
        else:
            (key1, key2), cost = _dict_argmin(distance_dict)
            distance_dict = _update_distance_dict(distance_dict,
                                                  distance_matrix,
                                                  key1,
                                                  key2)
            fusions.append((key1, key2, cost))
    return final_cluster, fusions


# http://biopython.org/wiki/Phylo
def phylo_to_ascii_dendrogram(linkage, consensus):
    """converts a linkage to a dendrogram
       inspired by Biopython's draw_ascii function"""
    pass

