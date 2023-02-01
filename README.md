# mastersProjectEsteban
These code was used for Esteban Echeverri's master's project at Grand Valley State University in the Winter 2022.

Three NP-hard graph theory problems are solved using a Brute Force approach:
- The Maximum Clique Problem
- The Minimum Dominating Set
- The Maximum Independent Set

The algorithms calculate the Power Set of the set vertices and check for every possible subset
if that subset is a possible solution for the problem being solved.

If a particular subset is a solution, its size is stored in an array.

After all the possible subsets have been evaluated, a reduction operation find the optimal solution
(minimum or maximum) depending on the problem.

This algorithm is not competetive with Branch-and-Bound solutions.
It might be of interest to illustrate how to take advantage of large number of processors on GPU
or as the basis for a quick and dirty solution for an NP-hard problem.
