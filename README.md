# LLSD
# Project Overview

This project is a graph algorithm designed for the Label-Constrained Shortest Path (LCSP) query problem, which is mainly used to efficiently process "shortest path queries satisfying label constraints" in complex graph structures.

The algorithm transforms the original graph data into an efficient queryable index structure by constructing a structured index and hierarchical representation of the graph, thereby significantly improving the efficiency of path queries under complex constraints. To further enhance scalability and preprocessing efficiency, the project incorporates a parallelized index construction mechanism, enabling multiple construction tasks to be executed concurrently and reducing index construction time, particularly for large-scale graphs.

## Project Objectives

Traditional shortest path algorithms (such as Dijkstra) have low efficiency when dealing with scenarios involving label constraints, multi-condition filtering, and other requirements. This project aims to address the following problems:

1. How to efficiently query the shortest path in graphs with label constraints  
2. How to reduce the search space explosion caused by complex constraints  
3. How to balance preprocessing efficiency and query efficiency  

## Project Structure

├── Graph.cpp / Graph.h      Graph structure construction and algorithm logic  
├── Tree.cpp / Tree.h        Hierarchical representation of the graph  
├── utility.cpp / utility.h  Basic data structures and utility functions  
├── test.cpp                 Experimental and testing entry point  

## How to Run

### Compilation Method
```js
g++ -std=c++17 -O2 Graph.cpp Tree.cpp utility.cpp test.cpp -o LLSD  
```

### Construction

```js
.\LLSD.exe [test set] [LSD index] [LLSD index] [hierarchical parameter] [test mode]
```
Read the graph and construct LSD index and LLSD table:
- [test set] represents the input undirected weighted graph dataset, formatted as ={number of edges, [source vertex, target vertex, edge weight, edge label], [source vertex, target vertex, edge weight, edge label]...}
- [LSD index] represents the name of the LSD index to be generated, including the decomposition tree structure
- [LLSD index] represents the name of the LLSD index to be generated
- [hierarchical parameter] represents the hierarchical parameter α, with a value range of 0-100
- [test mode] represents the execution mode, where 0 indicates index construction and 1 indicates data testing

### Search

```js
.\LLSD.exe [test set] [LSD index] [LLSD index] [hierarchical parameter] [test mode] [query set]
```
Read in graph, LSD index, LLSD table, and query dataset:
- [test set] represents the input undirected weighted graph dataset, formatted as ={number of edges, [source vertex, target vertex, edge weight, edge label], [source vertex, target vertex, edge weight, edge label]...}
- [LSD index] represents the name of the LSD index to be loaded, including the decomposition tree structure
- [LLSD index] represents the name of the LLSD index to be loaded
- [hierarchical parameter] represents the hierarchical parameter α, with a value range of 0-100
- [test mode] represents the execution mode, where 0 indicates index construction and 1 indicates data testing
- [query set] represents the name of the test set
