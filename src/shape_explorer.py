#! /usr/bin/env python
"""Abstract Shape Explorer for RNA suboptimal consensus solutions of MC-Cons"""


import argparse
from util import Consensus, read_consensus_file


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

