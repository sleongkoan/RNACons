# RNACons


## Description

RNACons is an algorithm that finds  RNA secondary structure consensus.
That is, with a list of sequences and potential structures, it tries to find
a group of structures, one per sequence, who have the greatest similarity between
all of them.

To do so, it first represents RNA secondary structures as base pair trees.
Base pair trees are ordered rooted trees built from the base pairs of a RNA
secondary structure. A special tree edit distance is used to compare these trees.
Potentially many equivalent consensus are found during this phase.


The second step refines the consensus previously created by minimizing the
string edit distance on the Vienna dot-bracket representation of RNA secondary
structure fitting the base pair trees. This step insures that unpaired regions
match as well as possible.


![](doc/article/figs/mccons_flowchart2.jpg)

## RNACons versions

Two versions of the consensus optimizer available,
one using an exact solver with branch and bound strategy and a heuristic
one using an hybrid genetic algorithm. Each solver can consider
suboptimal consensus solutions during the first phase to preserve more
variety in the final solutions.


## Shape Explorer
Shape explorer is a tool to explore RNACons outputs. This is useful mostly when RNACons
considers suboptimal solutions. It filters consensus by keeping only the best consensus
for each arrangement of abstract shapes level 5.


## Requirements

- Java JRE 7 or greater


## Input Files

Input files must be of the following fasta-like format.
That is it should look like this (here suboptimal\_i\_j
refers to the jth suboptimal structure of the ith molecule).

All suboptimal structures must be represented in Vienna dot bracket notation.

    >name_0
    sequence_0
    suboptimal_0_0
    suboptimal_0_1
    ...
    suboptimal_0_n
    >name_1
    sequence_1
    suboptimal_1_0
    suboptimal_1_1
    ...
    suboptimal_1_m
    >name_
    ...

Check out data/examples/mccons_example.marna for a real example.


## Output Format

Consensus are outputted in the following fasta-like format to standard
output (suboptimal\_n\_l would be the chosen suboptimal at index l of
the nth molecule).

The tree score and string edit score are the average
distance between all selected structures.

    > solution_index tree_score string_edit_score
    suboptimal_0_i
    suboptimal_1_j
    suboptimal_2_k
    ...
    suboptimal_n_l
    > solution_index tree_score string_edit_score
    ...

Check out data/examples/shape_explorer_example.marna for a real example.



## To Do
- [ ] pretty print the consensus
- [ ] tRNA y-shaped consensus (waiting for the sequences)
- [ ] RNAse P alignment http://www.mbio.ncsu.edu/rnasep/seqs&structures.html
