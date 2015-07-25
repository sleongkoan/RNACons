#! /usr/bin/env python
"""RNA abstract shape finder by Stephen Leong Koan"""
import argparse

class Consensus(object):
    def __init__(self, number, tree_dist, bp_dist, subopts):
        self.number = number
        self.tree_dist = tree_dist
        self.bp_dist = bp_dist
        self.subopts = subopts

    def __str__(self):
        ret = "> {0} {1} {2}\n".format(self.number, self.tree_dist, self.bp_dist)
        for subopt in self.subopts:
            ret += "{0}\n".format(subopt)
        ret += "\n"
        return ret
    def __repr__(self):
        return str(self)
    def __lt__(self, other):
        if self.tree_dist == other.tree_dist:
            return self.bp_dist < other.bp_dist
        else:
            return self.tree_dist < other.tree_dist
    def __gt__(self, other):
        return other < self
    def __eq__(self, other): # equality on score
        return self.tree_dist == other.tree_dist and self.bp_dist == other.bp_dist

    def to_shape5(self):
        shapes = []
        for subopt in self.subopts:
            shapes.append(find_shapes(subopt)[0]) # shape level 5 (most abstract)
        return shapes


def read_consensus(file_name):
    """read marna-like file format"""
    data = []
    with open(file_name) as data_file:
        subopts = []
        name = ""
        lines = data_file.readlines()
        for line in lines:
            if line.startswith(">"):  # new name
                if subopts:
                    data.append(Consensus(number, avg_tree_dist, avg_dotb_dist, subopts))
                split = line.strip().split(" ")
                number = int(split[1])
                avg_tree_dist = float(split[2])
                avg_dotb_dist = float(split[3])
                subopts = []
            if line[0] in ".()":  # new subopt
                subopts.append(line.strip())
            else:
                continue
        if subopts:  # end of file
            data.append(Consensus(number, avg_tree_dist, avg_dotb_dist, subopts))
    return data


def read_marna(file_name):
    """read marna-like file format"""
    data = []
    with open(file_name) as data_file:
        name = ""
        seq = ""
        subopts = []
        lines = data_file.readlines()
        for line in lines:
            if line.startswith(">"):  # new name
                if name != "":
                    data.append((name, seq, subopts))
                name = line.strip()[1:]  # remove ">" char
                seq = ""
                subopts = []
            stripped = line.strip()[0]
            if stripped in "AUGC":  # new sequence
                seq = line.strip()
            elif stripped in ".()":  # new subopt
                splitted = line.strip().split()
                subopt, _ = splitted[0], float(splitted[1])
                subopts.append(subopt)
            else:
                continue
        if name != "":  # end of file
            data.append((name, seq, subopts))
    return data


def find_shapes(structure):
    list_opener = []
    list_stems = []

    i = 0
    list_stem_start = []
    # separate in stems
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

    # build the level1 for each stems
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

    # assemble level1
    level1 = " "
    for i, element in enumerate(structure):
        level1 += dict_lvl1.get(str(i), "").strip()
        if element == "." and level1[-1] != "_" and not i in range_occupied:
            level1 += "_"

    level1 = level1.strip().replace("[_]", "[]")
    level1 = level1.replace(" ", "")
    # from level 1, build level 3 (remove unpaired symbols)
    level3 = level1.replace("_", "")
    level3 = level3.replace(" ", "")
    # from level 3, build level 5 by removing stems with bulges
    level5 = level3
    while level5.count("[[]]") > 0:
        level5 = level5.replace("[[]]", "[]")

    return level5, level3, level1


def get_unique_abstract_shapes(consensus):
    shape_to_consensus = dict()
    index = 0
    for elem in consensus:
        shapes = elem.to_shape5()
        shape_signature = "\n".join(shapes)
        if shape_signature in shape_to_consensus:  # already has key
            shape_to_consensus[shape_signature].append(elem)
        else:
            shape_to_consensus[shape_signature] = [elem]  # create new key
    # filter out per shape (shreps style)
    filtered = []
    for consensus_list in shape_to_consensus.itervalues():
        best_consensus = min(consensus_list)
        equal_list = []
        for elem in consensus_list:
            if elem == best_consensus:  # equal score
                equal_list.append(elem)

        filtered.extend(equal_list)

    filtered.sort()
    return filtered


if __name__ == '__main__':
    parser = argparse.ArgumentParser("A simple parser for filtering potential consensus by abstract shape representatives (shreps)")
    parser.add_argument('-cf',action="store", required=True, dest="consensus_file_path", help="path of the consensus file")
    parser.add_argument('-mf', action="store", required=True, dest="marna_file_path", help="path of the marna input file to the consensus")
    parser.add_argument('-o', action="store", dest="output_path", default=None, help="output file path (default is stdout)")
    args = parser.parse_args()

    marna_data = read_marna(args.marna_file_path)
    consensus = read_consensus(args.consensus_file_path)
    filtered = get_unique_abstract_shapes(consensus)
    for index, potential_consensus in enumerate(filtered):
        potential_consensus.number = index
    if args.output_path is None:
        for elem in filtered:
            print elem
    else:
        with open(args.output_path, "w") as writer:
            for elem in filtered:
                writer.write(elem)
