# EN2Link
A helper library for optimising water distribution networks. Evaluates constraints, costs, greenhouse gas emissions for pipe, pump and VSP options. Can specify number of decision variables and decision variable bounds for each of these.

Optimisation problem is specified within a text file that, using a format similar to EPNATE .inp files, specifies what links/pumps/reservoirs to optimise, what objectives to use, what pipe options to select from, and what EPANET library to use etc.

OPtimisation of water distribution networks using EPANET as solver engine for hydraulics. EPANET2, or Loez-Ibanez's multithread version are compatible, so is the development version of EPANET3.

Deigned for Parallel implementations of optimsiers (i.e. a parallel NSGAII as an example), as each process loads EPANET2 seperately into memory space

Multiplatform: compiles under Mac OS (clang), Windows (mingw/msvc), and Linux (gnu) tested.

Calculates cost of network, greenhouse gas emissions, embodied energy objectives
Calculates pressure and head constraints
According to formulation of objectives/constraints in configuration file

Would like to investigate whether functions can be compiled with mex for use with Matlab (R) without explicit loading of library from within Matlab environment
