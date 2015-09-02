#! /usr/bin/env python
"""interface with flashfold RNA secondary structure folding tool"""

import subprocess
import shlex
import argparse



def read_fasta(file_name):
    """read fasta-like file format"""
    data = []
    with open(file_name) as data_file:
        name = ""
        seq = ""
        lines = data_file.readlines()
        for line in lines:
            if line.startswith(">"):  # new name
                if name != "":
                    data.append((name, seq))
                name = line.strip()[1:]  # remove ">" char
                seq = ""
            stripped = line.strip()[0]
            if stripped in "AUGCTaugct":  # new sequence
                seq = line.strip().upper()
            else:
                continue
        if name != "":  # end of file
            data.append((name, seq))
    return data


def call_command(command, pipe=None, echo=False):
    """simple shell call interface for python"""
    if echo:
        print command

    process = subprocess.Popen(shlex.split(command.encode("ascii")),
                               stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)

    output = process.communicate(input=pipe)
    return output


def call_flashfold(seq,
                   num,
                   flashfold_path,
                   tables_path):
    """fold a sequence using flashfold"""
    command = "{0} -s {1} -ft {2} -tables {3}".format(flashfold_path,
                                                      seq, num,
                                                      tables_path)
    data = call_command(command)[0]
    to_list = data.split("\n")
    remove_graph = [i.split()[0]+" " + i.split()[1] for i in to_list if i != ""]
    return remove_graph


def fold_list(name_seq_list,
              num,
              flashfold_path,
              tables_path):
    """map flashfold accross a list of sequences"""
    folded = []
    for name, seq in name_seq_list:
        folded.append((name, seq, call_flashfold(seq, num, flashfold_path, tables_path)))
    return folded


def save_folded(destination, name_seq_subopts_list):
    """save a list of name; sequence; subopts to marna format"""
    with open(destination, "w") as output:
        for seq, name, subopts in name_seq_subopts_list:
            output.write(">" + seq + "\n")
            output.write(name + "\n")
            for subopt in subopts:
                output.write(subopt + "\n")
    return


if __name__ == '__main__':
    # create the command line parser
    PARSER = argparse.ArgumentParser("displays a dendrogram and an "\
                                     "heatmap of a consensus")

    # I/O PARAMETERS
    PARSER.add_argument('-i', action="store", required=True,
                        type=str, dest="in_path",
                        help="path of the fasta input file")

    PARSER.add_argument('-o', action="store", required=True,
                        type=str, dest="out_path",
                        help="output file path")

    # FOLDING PARAMETERS
    PARSER.add_argument("-n", action="store", default=10,
                        type=int, dest="num_subopts",
                        help="number of suboptimals structures wanted per sequence")

    PARSER.add_argument("-exec", action="store", default="../FlashFold/bin/flashfold",
                        type=str, dest="ff_path",
                        help="path of flashfold executable")
    PARSER.add_argument("-tables", action="store", default="../FlashFold/tables",
                        type=str, dest="tables_path",
                        help="path of the flashfold tables")

    # parse the arguments
    ARGS = PARSER.parse_args()

    # some input verification
    assert ARGS.num_subopts >= 0

    # fold all the sequences
    DATA = read_fasta(ARGS.in_path)
    FOLDED = fold_list(DATA, ARGS.num_subopts, ARGS.ff_path, ARGS.tables_path)

    # save them
    save_folded(ARGS.out_path, FOLDED)


