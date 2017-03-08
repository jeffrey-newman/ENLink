# EN2Link
A helper library for optimising water distribution networks. Evaluates constraints, costs, greenhouse gas emissions for pipe, pump and VSP options

OPtimisation of water distribution networks using EPANET2 as solver engine for hydraulics
Calculates cost of network, greenhouse gas emissions, embodied energy objectives
Calculates pressure and head constraints
According to formulation of objectives/constraints in configuration file
Deigned for Parallel implementations of optimsiers (i.e. a parallel NSGAII as an example), as each process loads EPANET2 seperately into memory space

Shared Library that has also been compiled for mex for use with Matlab (R).
