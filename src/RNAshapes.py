"""RNA abstract shape finder by Stephen Leong Koan"""
import argparse

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
    # from level 1, build level 3 (remove unpaired symbols)
    level3 = level1.replace("_", "")

    # from level 3, build level 5 by removing stems with bulges
    level5 = level3
    while level5.count("[[]]") > 0:
        level5 = level5.replace("[[]]", "[]")

    return level1, level3, level5

if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--structure',action="store", required=True, dest="structure")

    ns = parser.parse_args()

    structure = ns.structure

    level1, level3, level5 = find_shapes(structure)
    print level1
    print level3
    print level5
