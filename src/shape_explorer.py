#! /usr/bin/env python
"""Abstract Shape Explorer for RNA suboptimal consensus solutions of MC-Cons"""


import argparse


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


class Consensus(object):
    """simple object to hold all the information about a consensus"""
    def __init__(self, number, tree_dist, bp_dist, subopts, level):
        self.number = number
        self.tree_dist = tree_dist
        self.bp_dist = bp_dist
        for subopt in subopts:
            assert is_valid_dot_bracket(subopt), "problematic structure: {}".format(subopt)
        self.subopts = subopts
        self.abstract_shapes = ([dot_bracket_to_abstract_shape(subopt, level)
                                for subopt in self.subopts])

    def __str__(self):
        ret = "> {0} {1} {2}\n".format(self.number,
                                       self.tree_dist,
                                       self.bp_dist)
        for subopt in self.subopts:
            ret += "{0}\n".format(subopt)
        ret += "\n"
        return ret

    def __repr__(self):
        return str(self)

    def str_with_shapes(self):
        """string representation with abstract shapes along suboptimals"""
        ret = "> {0} {1} {2}\n".format(self.number,
                                self.tree_dist,
                                self.bp_dist)
        for subopt, shape in zip(self.subopts, self.abstract_shapes):
            ret += "{0} {1}\n".format(subopt, shape)
        ret += "\n"
        return ret

    def get_shape_signature(self):
        """return string of shapes separated by newline"""
        ret = ""
        for shape in self.abstract_shapes:
            ret += "{}\n".format(shape)
        return ret

    def __lt__(self, other):
        if self.tree_dist == other.tree_dist:
            return self.bp_dist < other.bp_dist
        else:
            return self.tree_dist < other.tree_dist

    def __gt__(self, other):
        return other < self

    def __eq__(self, other): # equality on score
        return ((self.tree_dist == other.tree_dist) and
                (self.bp_dist == other.bp_dist))


def read_consensus_file(file_name, level):
    """read consensus file made by MC-Cons (all versions)"""

    # data holders
    consensus = []
    number = -1
    avg_tree_dist = -1.
    avg_dotb_dist = -1.
    subopts = []

    with open(file_name) as data_file:

        for line in data_file:

            # starting new consensus
            if line.startswith(">"):
                if subopts:
                    consensus.append(Consensus(number,
                                               avg_tree_dist,
                                               avg_dotb_dist,
                                               subopts,
                                               level))
                split = line.strip().split(" ")
                number = int(split[1])
                avg_tree_dist = float(split[2])
                avg_dotb_dist = float(split[3])
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
                                       avg_tree_dist,
                                       avg_dotb_dist,
                                       subopts,
                                       level))
    return consensus


def dot_bracket_to_abstract_shape(structure, level):
    """Converts a Vienna dot-bracket structure into
       its corresponding abstract shape as defined in RNAshapes.
       Written by Stephen Leong Koan, IRIC, 2013"""
    # the 3 levels we use are the following
    # 1: Most accurate - all loops and all unpaired
    # 3: Nesting pattern for all loop types but no unpaired regions
    # 5: Most abstract - helix nesting pattern and no unpaired regions
    assert level in [1, 3, 5]
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
        range_occupied.extend(range_open+range_close)

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
    if level == 1:
        return level_1

    # from level 1, build level 3 (remove unpaired symbols)
    level_3 = level_1.replace("_", "")
    level_3 = level_3.replace(" ", "")
    if level == 3:
        return level_3

    # from level 3, build level 5 by removing stems with bulges
    level_5 = level_3
    while level_5.count("[[]]") > 0:
        level_5 = level_5.replace("[[]]", "[]")

    return level_5


def find_best_consensus_by_shapes(consensus_list):
    """Find the best consensus for each arrangement of shapes.
       Consensus are compared first by tree distance,
       if equal, we break the tie with the string edit distance.
       If the scores are identical to the best, we keep all."""
    assert isinstance(consensus_list, list)

    shape_to_consensus = dict()

    for consensus in consensus_list:
        shape_signature = consensus.get_shape_signature()
        if shape_signature in shape_to_consensus:  # already has key
            shape_to_consensus[shape_signature].append(consensus)
        else:
            shape_to_consensus[shape_signature] = [consensus]  # create new key

    # filter out, for each shape, by best score (keep equivalent)
    filtered = []
    for consensus_list in shape_to_consensus.itervalues():
        best_consensus = min(consensus_list)
        equal_list = []
        for consensus in consensus_list:
            if consensus == best_consensus:  # equal score
                equal_list.append(consensus)
        filtered.extend(equal_list)

    filtered.sort()
    return filtered


if __name__ == '__main__':
    # create the command line parser
    PARSER = argparse.ArgumentParser("filters consensus returned by MC-Cons " \
                                     "by abstract shape and consensus scores")

    PARSER.add_argument('-i', action="store", required=True,
                        type=str, dest="consensus_file_path",
                        help="path of the consensus file returned by MC-Cons")

    PARSER.add_argument('-o', action="store", required=False,
                        type=str, dest="output_path", default="stdout",
                        help="output file path (default is stdout)")

    PARSER.add_argument('-l', action="store", required=False,
                        type=int, dest="level", default=5, choices=[1, 3, 5],
                        help="abstract shape level to use for filtering")

    PARSER.add_argument('-s', action="store_true",
                        dest="display_abstract_shape", default=False,
                        help="prints the suboptimals with abstract shapes")

    # parse the arguments
    ARGS = PARSER.parse_args()

    # filter out the suboptimal consensus for each arrangement of shapes
    CONSENSUS_LIST = read_consensus_file(ARGS.consensus_file_path, ARGS.level)
    FILTERED_CONSENSUS = find_best_consensus_by_shapes(CONSENSUS_LIST)

    # renumber the selected consensus
    for INDEX, SELECTED_CONSENSUS in enumerate(FILTERED_CONSENSUS):
        SELECTED_CONSENSUS.number = INDEX

    # choose how the consensus will be converted to string
    # (with or without abstract shapes)
    STRFN = lambda x: x.__str__()
    if ARGS.display_abstract_shape == True:
        STRFN = lambda x: x.str_with_shapes()

    # output to either stdout or specified file
    if ARGS.output_path == "stdout":
        for SELECTED_CONSENSUS in FILTERED_CONSENSUS:
            print STRFN(SELECTED_CONSENSUS)
    else:
        with open(ARGS.output_path, "w") as writer:
            for SELECTED_CONSENSUS in FILTERED_CONSENSUS:
                writer.write(STRFN(SELECTED_CONSENSUS))

