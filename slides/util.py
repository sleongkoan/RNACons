"""interface with flashfold RNA secondary structure folding tool"""

import copy
import subprocess
import shlex
import xml.etree
import math

VARNA_PATH = "../../VARNAv3-93.jar"

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


def read_data(file_name):
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

    call_VARNA(name, sequence, structure, bpcolor, options='-border "20x20" -periodNum {0} -baseInner #ffffff -baseOutline #ffffff -baseName #ffffff -baseNum #ffffff -bpStyle line -o "{1}_step2.svg"'.format(len(sequence), name))
    return


def apply_steps(data, color):
    for name, seq, struct in data:
        step1(name, seq, struct, color)
        step2(name, seq, struct, color)
    return


def get_bpsize(image_path, bpstroke="rgb(0%, 0%, 100%)"):
    """calculate the base pair size given a varna generated svg image"""
    # parse the image
    xml_tree = xml.etree.ElementTree.parse(image_path)
    root = xml_tree.getroot()

    # find the size of a base pair (by color and name)
    elems = root.getchildren()
    linesize = None
    for elem in elems:
        if (elem.tag.find("line") != -1) and (elem.attrib["stroke"] == bpstroke):
            x1 = float(elem.attrib["x1"])
            x2 = float(elem.attrib["x2"])
            y1 = float(elem.attrib["y1"])
            y2 = float(elem.attrib["y2"])
            linesize = math.sqrt((x2 - x1)**2 + (y2 - y1)**2)
            break
    return linesize


def scaleSVG(image_path, base_pair_length, bpstroke="rgb(0%, 0%, 100%)"):
    """scale the svg in such a way that the base pair
    size is the same as the one given as argument"""
    # parse the image
    xml_tree = xml.etree.ElementTree.parse(image_path)
    root = xml_tree.getroot()

    # find the size of a base pair (by color and name)
    elems = root.getchildren()
    linesize = None
    for elem in elems:
        if (elem.tag.find("line") != -1) and (elem.attrib["stroke"] == bpstroke):
            x1 = float(elem.attrib["x1"])
            x2 = float(elem.attrib["x2"])
            y1 = float(elem.attrib["y1"])
            y2 = float(elem.attrib["y2"])
            linesize = math.sqrt((x2 - x1)**2 + (y2 - y1)**2)
            break
    assert(not linesize is None)

    # calculate the ratio
    scale = linesize / float(base_pair_length)
    # set the scale transform
    root.attrib["transform"] = "scale({0})".format(scale)

    xml_tree.write(image_path)
    return
