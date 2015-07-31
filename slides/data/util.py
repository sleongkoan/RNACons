"""interface with flashfold RNA secondary structure folding tool"""

from collections import Counter
import copy
import math
import matplotlib.cm
import matplotlib.colors
import os
import subprocess
import shlex
from xml.dom import minidom


VARNA_PATH = "VARNAv3-93.jar"

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


def preprocess_data(data, limit_subopts=float("inf")):
    """applies some modifs to data, making it easier to work with VARNA"""
    data = copy.copy(data)  # eliminate side effects

    # limit the number of subopts if wished
    if limit_subopts < float("inf"):
      data = [(i[0], i[1], i[2][:limit_subopts]) for i in data]

    # put it in list format (easier to call varna with this)
    data_list = []
    for name, seq, subopts in data:
      tmp = []
      for i, subopt in enumerate(subopts):
        tmp.append((name+"_{}".format(i), seq, subopt))
      data_list.append(tmp)
    return data_list


def read_consensus(file_name):
    """read marna-like file format"""
    data = []
    with open(file_name) as data_file:
        subopts = []
        lines = data_file.readlines()
        for line in lines:
            if line.startswith(">"):  # new name
                if subopts:
                    data.append(subopts)
                subopts = []
            if line[0] in ".()":  # new subopt
                subopts.append(line.strip())
            else:
                continue
        if subopts:  # end of file
            data.append(subopts)
    return data


def call_VARNA(name, sequence, structure, bpcolor, options=""):
    """calls varna with given options"""

    # compose the call
    varna_call = "java -cp {0} fr.orsay.lri.varna.applications.VARNAcmd ".format(VARNA_PATH)
    call_params = "-sequenceDBN '{1}' -structureDBN '{2}' -bp {3} {4}".format(name, sequence, structure, bpcolor, options)
    full_call = varna_call + call_params

    # print and call
    print "Calling {}".format(full_call)
    call_command(full_call)

    return


def step1(name, sequence, structure, bpcolor):
    """creates illustration for step 1 of the demonstration"""
    call_VARNA(name, sequence, structure, bpcolor, options='-periodNum {0} -o "{1}_step1.svg"'.format(len(sequence), name))
    return



def step2(name, sequence, structure, bpcolor):
    """creates illustration for step 1 of the demonstration, only the skeleton"""
    # eliminate bulges
    structure = copy.copy(structure)
    structure = structure.replace(".", "")
    structure = structure.replace("()", "(..)")
    #structure = "(" + structure + ")"  # add artificial root to make it a tree
    sequence = "".join(["A" for _ in structure])

    call_VARNA(name, sequence, structure, bpcolor, options='-border "20x20" -periodNum {0} -baseInner #ffffff -baseOutline #ffffff -baseName #ffffff -baseNum #ffffff -bpStyle line -o "{1}_step2.svg"'.format(len(sequence)+1, name))
    return


def apply_steps(data, color):
    for name, seq, struct in data:
        step1(name, seq, struct, color)
        step2(name, seq, struct, color)
    return


def remove_text_nodes(file_path):
    """ Removes all of the whitespace-only text decendants of a DOM node. """
    # prepare the list of text nodes to remove (and recurse when needed)

    doc = minidom.parse(file_path)  # parseString also exists
    text = doc.getElementsByTagName('text')

    # perform the removals
    for node in text:
        node.parentNode.removeChild(node)
        node.unlink( )

    # write to file the new xml tree
    with open(file_path, "w") as writer:
        doc.writexml(writer)

    return


def concat_images(svg_paths, destination):
    """extracts svg nodes, create groups and puts them into a new svg"""
    # extract all svg elements
    docs = [minidom.parse(svg_path) for svg_path in svg_paths]
    svgs = [doc.getElementsByTagName("svg") for doc in docs]
    svgs = [item for sublist in svgs for item in sublist if sublist]

    # remove attributes
    for svg in svgs:
        for key in svg.attributes.keys():
            svg.removeAttribute(key)
        # change it to a grouping
        svg.tagName = u'g'


    final_svg = minidom.Element("svg")
    for group in svgs:
        final_svg.appendChild(group)

    # write doc1 to destination
    with open(destination, "w") as writer:
        final_svg.writexml(writer)

    return


def pretty_print_consensus(consensus_data, sequences, destination, cmap='copper'):
    """pretty print the first consensus of a problem using VARNA"""

    assert isinstance(consensus_data[0], str), "must be a single consensus"
    hex_color = "#000000"

    file_names = []
    for index in range(len(consensus)):
        new_name = "{0}_{1}".format(index, "tmp")
        struct = consensus_data[index]
        seq = sequences[index]

        step1(new_name, seq, struct, hex_color)
        file_name = new_name+ "_step1.svg"
        remove_text_nodes(file_name)
        file_names.append(file_name)

    # concatenate into a single picture and remove all intermediates
    concat_images(file_names, destination)
    return file_names  # for later deletion if needed


def delete_files(file_names):
    for file_name in file_names:
        os.remove(file_name)
    return
