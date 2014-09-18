This is the program of my summer school project 2014.

This is an implementation and experiments of the 
Contraction Hierachies(CH) algorithm, which is used 
to find the shortest (optimal) path on a continental-sized 
map (could be millions of nodes).

The CH algorithm is particularly suitable for sparse 
large planar graphs like road networks.

It is claimed that CH algorithm is one to two orders 
of magnitude faster than Dijkstra's algorithm. Our 
goal of this project is to verify the speedup. 

Our implementation of CH algorithm and the comparisons 
of CH query algorithm to the classical Dijkstra's algorithm 
by experiments have verified the efficiency of CH preprocessing 
algorithm and CH query algorithm.

The CH query algorithm, which can find a shortest path in 85ms 
on the whole USA road networks.

Summary:
24 million nodes, 58 million edges, 85ms
3 algorithms, 3 data structures, 3 operating systems
C++ & Java (4000 lines)
6 innovations:
      A new performance analysis on a famous data structure
      A new way for data preprocessing
      A fast algorithm for finding shortcuts in a large graph
      A more efficient path composition method
      A new proof for theory foundations and rectified a fault 
            in the previous proof
      A new algorithm for the problem

More details can be found in this report. 

Deqi Li, 2014


