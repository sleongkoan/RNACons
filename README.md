# MC-Cons


## Description

There are currently two versions available, one using an hybrid
Genetic Algorithm and the other one a Monte Carlo coupled with
steepest descent.

All results are outputted to standard output (stdout).


### Input files

Input files must be of the following fasta-like format.
That is it should look like this (here suboptimal\_i\_j
refers to the jth suboptimal structure of the ith molecule).


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


### Output format

Consensus are outputted in the following fasta-like format
(suboptimal\_n\_l would be the chosen suboptimal
at index l of the nth molecule).


    > solution_index tree_score string_edit_score
    suboptimal_0_i
    suboptimal_1_j
    suboptimal_2_k
    ...
    suboptimal_n_l
    > solution_index tree_score string_edit_score
    ...

## Compilation Instructions

Using the provided makefile, just (seems to work fine on Linux and OSX)

```
make all
```
