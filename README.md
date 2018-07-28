Elf Protein Structure Prediction - Parallel ABC Algorithm
===

<img src="img/elf_icon.png" width="128" height="128">

Implementations of a Protein Structure Prediction (PSP) program, using an Artificial Bee Colony optimization algorithm.


Overview
---

In this repository, I implement the protein structure prediction program described by Professor César Benítez and Professor Heitor Lopes in their article "Parallel Artificial Bee Colony Algorithm Approaches for Protein Structure Prediction Using the 3DHP-SC Model", whose complete reference is given below.

> Benítez, C.M.V. and Lopes, H.S., 2010. Parallel artificial bee colony algorithm approaches for protein structure prediction using the 3dhp-sc model. In Intelligent Distributed Computing IV (pp. 255-264). Springer, Berlin, Heidelberg.

The authors provide 2 parallelizations of a sequential PSP program that is described very thoroughly in their article. One of the parallelizations consist in splitting the work among nodes who communicate among themselves in a master-slave fashion; the other version (called Hybrid Hierarchical) splits the work among many master-slave systems, each of which work exactly the same way as the first parallelization described, and the masters communicate among themselves periodically in a ring logical topology. I only implemented the Hybrid Hierarchical version, because it can work exactly the same way as the master-slave version if you configure its parameters accordingly.

I've been investigating their proposal due to my research project, and during analysis I found out that I could greatly improve the execution time of the program by modifying one internal aspect of their algorithm. The modification consists on reducing the complexity of a hotspot of the algorithm, from O(n^2) to O(n). This resulted in the implementation of 4 different versions of the program:

- Sequential Quadratic: sequential version of the PSP algorithm, using the regular quadratic-complexity procedures;

- Sequential Linear: sequential version of the PSP algorithm, using the linear-complexity procedures I devised;

- Parallel Quadratic: Hybrid Hierarchical version of the PSP algorithm, using the regular quadratic-complexity procedures;

- Parallel Linear: Hybrid Hierarchical version of the PSP algorithm, using the linear-complexity procedures I devised.


Proposal for Complexity Reduction
---


Requirements
---


Compiling
---


Usage
---


Configuring Internal Parameters
---
