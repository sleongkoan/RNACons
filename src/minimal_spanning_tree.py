"""applying Prim's algorithm on a distance matrix (pure python)"""


import copy


class LabeledDistanceMatrix(object):
    """keeps labels, distance matrix, and a min heap"""
    def __init__(self, distance_matrix, labels=None):
        # check that the matrix is symmetric
        if not distance_matrix_is_symmetric(distance_matrix):
            raise Exception("Non symmetric distance matrix")

        # exclude self connections
        self.distance_matrix = copy.copy(distance_matrix)
        for i in range(len(self.distance_matrix)):
            self.distance_matrix[i][i] = float('inf')
        self.labels = labels


    def __str__(self):
        """ display the distance matrix and its fields"""
        to_str = "Distance Matrix\n"
        if not self.labels is None:
            to_str += "Labels : {}\n".format(self.labels)
            to_str += "\n"
        for i in self.distance_matrix:
            to_str += str(i)
            to_str += "\n"
        return to_str


    def __repr__(self):
        return str(self)


    def update_distance_matrix(self, row):
        """set all the distance on the row to infinity"""
        for i in range(len(self.distance_matrix)):
            self.distance_matrix[row][i] = float('inf')
        return


    def find_min_arc(self, selected_columns):
        """search for best feasible arc and update the matrix"""
        best_column = -1
        best_row = -1
        best_distance = float('inf')
        for column in selected_columns:
            for row in range(len(self.distance_matrix)):
                current_dist = self.distance_matrix[row][column]
                if current_dist < best_distance:
                    best_distance = current_dist
                    best_column = column
                    best_row = row
        # kill the row
        self.update_distance_matrix(best_row)
        return (best_column, best_row)


def distance_matrix_is_symmetric(distance_matrix):
    """checks wether or not the distance matrix is symmetric"""
    is_symmetric = True
    for i in range(len(distance_matrix)):
        if is_symmetric:
            for j in range(len(distance_matrix)):
                if distance_matrix[i][j] != distance_matrix[j][i]:
                    is_symmetric = False
                    break
    return is_symmetric


def prim_mst(labelled_matrix):
    """find the MST using Prim's algorithm"""
    assert isinstance(labelled_matrix, LabeledDistanceMatrix)
    min_spanning_tree = []
    current_columns = [0]
    labelled_matrix.update_distance_matrix(0)
    num_nodes = len(labelled_matrix.distance_matrix)
    arc1 = 0
    arc2 = -1
    # main iteration
    for _ in range(num_nodes -1):
        arc1, arc2 = labelled_matrix.find_min_arc(current_columns)
        min_spanning_tree.append((arc1, arc2))
        current_columns.append(arc2)
    return min_spanning_tree


def test_prim():
    """simple test"""
    test_data = [[0, 2, 4, 6, 6, 8],
                 [2, 0, 4, 6, 6, 8],
                 [4, 4, 0, 6, 6, 8],
                 [6, 6, 6, 0, 4, 8],
                 [6, 6, 6, 4, 0, 8],
                 [8, 8, 8, 8, 8, 0]]
    distance_matrix = LabeledDistanceMatrix(test_data)
    min_spanning_tree = prim_mst(distance_matrix)
    print min_spanning_tree
    return


