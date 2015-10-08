"""Objects and functions used in multiple scripts"""


import collections


def is_valid_dot_bracket(dot_bracket):
    """tests Vienna dot-bracket for illegal structure (or symbol)"""
    counter = 0
    for i in dot_bracket:
        if i == '(':
            counter += 1
        elif i == ')':
            counter -= 1
        elif i != '.':  # illegal symbol
            return False
        if counter < 0:  # unbalanced structure
            return False
    if counter != 0:
        return False  # unbalanced structure
    return True


def dot_bracket_to_abstract_shape(structure):
    """Converts a Vienna dot-bracket structure into
       its corresponding abstract shape as defined in RNAshapes.
       Written by Stephen Leong Koan, IRIC, 2013"""
    # the 3 levels we use are the following
    # 1: Most accurate - all loops and all unpaired
    # 3: Nesting pattern for all loop types but no unpaired regions
    # 5: Most abstract - helix nesting pattern and no unpaired regions
    list_opener = []

    list_stems = []
    i = 0
    list_stem_start = []

    # separate into stems
    while i < len(structure):
        if structure[i] == "(":
            list_opener.append(i)
            if not list_stem_start:
                list_stem_start.append(i)
        elif structure[i] == ")":
            current_stem = dict(opener=[], closer=[], pairs=[])
            while i < len(structure):
                if structure[i] == ")":
                    closer = list_opener.pop(-1)
                    current_stem["opener"].append(closer)
                    current_stem["closer"].append(i)
                    current_stem["pairs"].append((closer, i))
                    if closer in list_stem_start:
                        break
                elif structure[i] == "(":
                    list_stem_start.append(i)
                    i -= 1
                    break
                i += 1
            list_stems.append(current_stem)
        i += 1

    # build the level 1 for each stems
    range_occupied = []
    dict_lvl1 = dict()
    for stem in list_stems:
        range_open = range(min(stem['opener']), max(stem['opener'])+1)
        range_close = range(min(stem['closer']), max(stem['closer'])+1)
        range_occupied.extend(range_open + range_close)

        temp_lvl1_open = " "
        for i in range_open:
            if structure[i] == "(" and temp_lvl1_open[-1] != "[":
                temp_lvl1_open += "["
            elif structure[i] == "." and temp_lvl1_open[-1] != "_":
                temp_lvl1_open += "_"

        temp_lvl1_close = " "
        for i in range_close:
            if structure[i] == ")" and temp_lvl1_close[-1] != "]":
                temp_lvl1_close += "]"
            elif structure[i] == "." and temp_lvl1_close[-1] != "_":
                temp_lvl1_close += "_"

        while temp_lvl1_open.count("[") < temp_lvl1_close.count("]"):
            temp_lvl1_open = "[" + temp_lvl1_open

        while temp_lvl1_open.count("[") > temp_lvl1_close.count("]"):
            temp_lvl1_close += "]"

        dict_lvl1[str(min(stem['opener']))] = temp_lvl1_open
        dict_lvl1[str(min(stem['closer']))] = temp_lvl1_close

    # assemble level 1
    level_1 = " "
    for i, element in enumerate(structure):
        level_1 += dict_lvl1.get(str(i), "").strip()
        if element == "." and level_1[-1] != "_" and not i in range_occupied:
            level_1 += "_"

    level_1 = level_1.strip().replace("[_]", "[]")
    level_1 = level_1.replace(" ", "")

    # from level 1, build level 3 (remove unpaired symbols)
    level_3 = level_1.replace("_", "")
    level_3 = level_3.replace(" ", "")

    # from level 3, build level 5 by removing stems with bulges
    level_5 = level_3
    while level_5.count("[[]]") > 0:
        level_5 = level_5.replace("[[]]", "[]")

    return (level_5, level_3, level_1)


class Node(object):
    """ A simple node object for ordered tree representation"""
    def __init__(self, parent, label=-1):
        if parent is not None:
            assert isinstance(parent, Node)
            parent.children.append(self)
        self.label = label
        self.children = list()
        self.parent = parent

    def append(self, other_node):
        """add the other_node and the end of the node's children list"""
        self.children.append(other_node)


def inorder_helper(node, brackets):
    """inorder traversal of the node, to get a dotbracket representation"""
    brackets.append("(")
    for child in node.children:
        inorder_helper(child, brackets)
    brackets.append(")")
    return


def get_lmd(node):
    """get the id of the leftmost descendent"""
    while len(node.children) != 0:
        node = node.children[0]
    return node.label


def get_post_order_nodes(root):
    """returns an array of the nodes in postorder traversal order"""

    def get_postorder(node, res):
        """helper for outer function"""
        for children in node.children:
            get_postorder(children, res)
        res.append(node)
        return
    result = list()
    get_postorder(root, result)
    return result


class Tree(object):
    """ tree object used for the computation of tree edit distance"""
    def __init__(self, root):
        """build the tree with annotation for zhang-shasha algorithm"""

        # root of the tree
        self.root = root

        # bracket representation of the tree
        bracket_list = list()
        inorder_helper(self.root, bracket_list)
        # remove the artificial root added for convenience
        bracket_list.pop(-1)
        bracket_list.pop(0)
        self.brackets = "".join(bracket_list)

        # postorder enumeration of the nodes and labeling
        self.nodes = get_post_order_nodes(root)
        for index, node in enumerate(self.nodes):
            node.label = index

        # left most descendents
        self.lmds = [0 for _ in range(len(self.nodes))]

        # figure out the leftmost descendents for each nodes
        for node in self.nodes:
            self.lmds[node.label] = get_lmd(node)

        # figure out the keyroots, {(max k | lmd(k) = x) forall x}
        tmp = collections.defaultdict(lambda: -1)
        for node in reversed(self.nodes):
            index = self.lmds[node.label]
            if tmp[index] == -1:
                tmp[index] = node.label
        self.keyroots = sorted(tmp.values())
        return

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return self.brackets

    def __eq__(self, other):
        return self.brackets == other.brackets

    def __hash__(self):
        return hash(str(self))


def treedist(tree_1, tree_2, node_index_1, node_index_2, treedists):
    """calculate the tree edit distance by dynamic programming"""

    num_nodes_a = node_index_1 - tree_1.lmds[node_index_1] + 2
    num_nodes_b = node_index_2 - tree_2.lmds[node_index_2] + 2
    forest_distance = [[0 for _ in range(num_nodes_b)]
                       for _ in range(num_nodes_a)]

    ioff = tree_1.lmds[node_index_1] - 1
    joff = tree_2.lmds[node_index_2] - 1

    # fill deletions (first row)
    for index_1 in range(1, num_nodes_a):
        forest_distance[index_1][0] = forest_distance[index_1-1][0] + 1

    # fill insertions (first column)
    for index_2 in range(1, num_nodes_b):
        forest_distance[0][index_2] = forest_distance[0][index_2-1] + 1

    for index_1 in range(1, num_nodes_a):
        for index_2 in range(1, num_nodes_b):
            # case 1 : index_1 is an ancestor of i and y is an ancestor of j
            if ((tree_1.lmds[node_index_1] == tree_1.lmds[index_1 + ioff]) and
                (tree_2.lmds[node_index_2] == tree_2.lmds[index_2 + joff])):
                forest_distance[index_1][index_2] = (min
                               (forest_distance[index_1-1][index_2] + 1,
                                forest_distance[index_1][index_2-1] + 1,
                                forest_distance[index_1-1][index_2-1]))
                treedists[index_1+ioff][index_2+joff] = (
                              forest_distance[index_1][index_2])

            # case 2
            else:
                index_3 = tree_1.lmds[index_1+ioff]-1-ioff
                index_4 = tree_2.lmds[index_2+joff]-1-joff
                forest_distance[index_1][index_2] = min(
                                      (forest_distance[index_1-1][index_2] + 1),
                                      (forest_distance[index_1][index_2-1] + 1),
                                      (forest_distance[index_3][index_4] +
                                        treedists[index_1+ioff][index_2+joff]))


def unlabeled_distance(tree_1, tree_2):
    """unlabeled tree distance"""
    treedists = [[0 for _ in range(len(tree_2.nodes))]
                 for _ in range(len(tree_1.nodes))]

    for index1 in tree_1.keyroots:
        for index2 in tree_2.keyroots:
            treedist(tree_1, tree_2, index1, index2, treedists)

    return treedists[-1][-1]


def only_paired(dot_bracket):
    """remove '.' character from dot_bracket"""
    assert is_valid_dot_bracket(dot_bracket), "invalid dot-bracket"
    return "".join([i for i in dot_bracket if i != '.'])


def dot_bracket_to_tree(dot_bracket):
    """creates a abstract shape base pair tree from the Vienna dot bracket"""
    assert is_valid_dot_bracket(dot_bracket), "invalid dot-bracket"
    root = Node(None)
    position = root
    for symbol in dot_bracket:
        if symbol == '(':
            child = Node(position)
            position = child
        elif symbol == ')':
            position = position.parent
        else:
            continue
    return Tree(root)


def levenshtein(string_1, string_2):
    "Calculates the Levenshtein distance between a and b."
    # distance written by Magnus Lie Hetland

    length_1, length_2 = len(string_1), len(string_2)
    if length_1 > length_2:
        # make sure n <= m, to use O(min(n,m)) space
        string_1, string_2 = string_2, string_1
        length_1, length_2 = length_2, length_1

    current = range(length_1 + 1)
    for index_1 in range(1, length_2 + 1):
        previous, current = current, [index_1] + [0] * length_1
        for index_2 in range(1, length_1 + 1):
            add, delete = previous[index_2] + 1, current[index_2 - 1] + 1
            change = previous[index_2 - 1]
            if string_1[index_2 - 1] != string_2[index_1 - 1]:
                change = change + 1
            current[index_2] = min(add, delete, change)

    return current[length_1]


def matrix_average(matrix):
    """compute the arithmetic mean of the matrix"""
    total = 0.
    num_elems = 0
    for row in matrix:
        for value in row:
            num_elems += 1
            total += value
    return total / num_elems


class Consensus(object):
    """simple object to hold all the information about a consensus"""
    def __init__(self, number, subopts):
        assert number >= 0
        assert isinstance(subopts, list)
        for subopt in subopts:
            assert is_valid_dot_bracket(subopt)

        # assign basic fields
        self.number = number
        self.subopts = subopts
        self.abstract_shapes = ([dot_bracket_to_abstract_shape(subopt)
                                for subopt in self.subopts])
        # compute the distance matrices
        self.tree_dists = [[0. for _ in range(len(self.subopts))]
                                  for _ in range(len(self.subopts))]
        self.string_dists = [[0. for _ in range(len(self.subopts))]
                                    for _ in range(len(self.subopts))]
        self.trees = [dot_bracket_to_tree(subopt) for subopt in self.subopts]
        for index_1, subopt_1 in enumerate(self.subopts):
            for index_2, subopt_2 in enumerate(self.subopts):
                self.tree_dists[index_1][index_2] = (unlabeled_distance(
                                                     self.trees[index_1],
                                                     self.trees[index_2]))
                self.string_dists[index_1][index_2] = levenshtein(subopt_1,
                                                                  subopt_2)
        return

    def __str__(self):
        avg_tree_dist   = matrix_average(self.tree_dists)
        avg_string_dist = matrix_average(self.string_dists)
        ret = "> {0} {1:.3f} {2:.3f}\n".format(self.number,
                                       avg_tree_dist,
                                       avg_string_dist)
        for subopt in self.subopts:
            ret += "{0}\n".format(subopt)
        ret += "\n"
        return ret

    def __repr__(self):
        return str(self)

    def str_with_shapes_order(self, level):
        """string representation with abstract shapes along suboptimals"""
        assert level in [5, 3, 1]
        level_to_index = [None, 2, None, 1, None, 0]
        access_index = level_to_index[level]

        avg_tree_dist   = matrix_average(self.tree_dists)
        avg_string_dist = matrix_average(self.string_dists)

        ret = "> {0} {1:.3f} {2:.3f}\n".format(self.number,
                                               avg_tree_dist,
                                               avg_string_dist)
        order = consensus_to_order(self)
        for index in order:
            subopt = self.subopts[index]
            shape = self.abstract_shapes[index][access_index]
            ret += "{0} {1} {2}\n".format(subopt, index, shape)
        ret += "\n"
        return ret


    def get_shape_signature(self, level):
        """return string of shapes separated by newline"""
        assert level in [5, 3, 1]
        level_to_index = [None, 2, None, 1, None, 0]
        access_index = level_to_index[level]
        ret = ""
        for shape in self.abstract_shapes:
            ret += "{}\n".format(shape[access_index])
        return ret


    def __lt__(self, other):
        if matrix_average(self.tree_dists) == matrix_average(other.tree_dists):
            return (matrix_average(self.string_dists) <
                    matrix_average(other.string_dists))
        else:
            return (matrix_average(self.tree_dists) <
                    matrix_average(other.tree_dists))


    def __gt__(self, other):
        return other < self


    def __eq__(self, other): # equality on score
        equal_tree_dists = (matrix_average(self.tree_dists) ==
                              matrix_average(other.tree_dists))
        equal_string_dists = (matrix_average(self.string_dists) ==
                                matrix_average(other.string_dists))
        return equal_tree_dists and equal_string_dists



def read_consensus_file(file_name,
                        index=None):
    """read consensus file made by MC-Cons (all versions)"""

    # data holders
    consensus = []
    number = -1
    subopts = []
    if index is None:
        with open(file_name) as data_file:

            for line in data_file:

                # starting new consensus
                if line.startswith(">"):
                    if subopts:
                        consensus.append(Consensus(number,
                                                  subopts))
                    split = line.strip().split(" ")
                    number = int(split[1])
                    subopts = []

                # reading a consensus structure
                if line[0] in ".()":
                    subopts.append(line.strip())

                # reading whitespace of something else (comments?)
                else:
                    continue

            # last consensus (if the file wasn't empty)
            if subopts:
                consensus.append(Consensus(number,
                                          subopts))
    else:
        current_index = -1
        with open(file_name) as data_file:
            for line in data_file:
                # starting new consensus
                if line.startswith(">"):
                    current_index += 1
                    if index == current_index and subopts:
                        return Consensus(number, subopts)
                    split = line.strip().split(" ")
                    number = int(split[1])
                    subopts = []

                # reading a consensus structure
                if line[0] in ".()":
                    subopts.append(line.strip())

                # reading whitespace of something else (comments?)
                else:
                    continue

            # last consensus (if the file wasn't empty)
            if subopts:
                return Consensus(number, subopts)
    return consensus



