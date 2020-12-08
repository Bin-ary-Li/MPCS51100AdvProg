# Homework 6

# Name: Bin Li

# Introduction

The program finds the top 10 longest paths in a graph (provided in files like `graph1.txt` and `graph2.txt`). Compile the program with `make` to get the program `hw6`. The program `hw6` can take two arguments, `./hw6 <filename> <threadcount>`, which specify the graph file to search and the number of thread to use for OpenMP. Default to `./findpath graph1.txt <max thread>`, i.e., finding top 10 longest paths in file `graph1.txt` using maximum number of threads available via OpenMP method `omp_get_max_threads()`.

# Verification

The implementation of the dijkstra shortest path algorithm with min heap priority queue is verified by comparing against the output of the dijkstra algorithm with vanilla serial search (instead of using min heap, looping through all vertices to find the next closest vertices). 

Output of Dijkstra with min heap to find the longest path of the first 1000 vertices in the file graph1.txt: 
```
----------------------------------
The top 10 longest paths: 
----------------------------------
path from 644 to 6278 with distance 192
path from 644 to 6290 with distance 186
path from 645 to 6278 with distance 184
path from 644 to 6187 with distance 183
path from 644 to 6188 with distance 181
path from 644 to 6277 with distance 180
path from 573 to 6278 with distance 179
path from 232 to 6278 with distance 179
path from 787 to 6278 with distance 178
path from 645 to 6290 with distance 178
----------------------------------
Running time: 0.978556 s (~0 min)
----------------------------------
```

Output of Dijkstra with serial search to find the longest path of the first 1000 vertices in the file graph1.txt:
```
----------------------------------
The top 10 longest paths: 
----------------------------------
path from 644 to 6278 with distance 192
path from 644 to 6290 with distance 186
path from 645 to 6278 with distance 184
path from 644 to 6187 with distance 183
path from 644 to 6188 with distance 181
path from 644 to 6277 with distance 180
path from 573 to 6278 with distance 179
path from 232 to 6278 with distance 179
path from 787 to 6278 with distance 178
path from 645 to 6290 with distance 178
----------------------------------
Running time: 62.614049 s (~1 min)
----------------------------------
```

# Program Output and Performance Comparison

## Quick overview
```
Filename,   Running Time,             Thread Count
graph1.txt, 4.844837s,                1
graph1.txt, 0.908267s,                6
graph2.txt, 6572.489423s (~109 min),  1
graph2.txt, 1919.495081s (~31 min),   6
```

Program running to find the top 10 longest paths in the small graph file (`graph1.txt`) using serial operation (running dijkstra serially on every source vertex): 
```
----------------------------------
The top 10 longest paths: 
----------------------------------
path from 644 to 6278 with distance 192
path from 5720 to 6278 with distance 186
path from 644 to 6290 with distance 186
path from 645 to 6278 with distance 184
path from 3634 to 6278 with distance 184
path from 644 to 6187 with distance 183
path from 4568 to 6278 with distance 183
path from 5995 to 6278 with distance 181
path from 644 to 6188 with distance 181
path from 4193 to 6278 with distance 181
----------------------------------
Running time: 4.844837 s (~0 min)
----------------------------------
```

Program running to find the top 20 longest paths in the small graph file (`graph1.txt`) with OpenMP and dynamic scheduling (6 core Xeon E5-1650, max number of concurrent threads = 6): 
```
----------------------------------
The top 10 longest paths: 
----------------------------------
path from 644 to 6278 with distance 192
path from 5720 to 6278 with distance 186
path from 644 to 6290 with distance 186
path from 645 to 6278 with distance 184
path from 3634 to 6278 with distance 184
path from 644 to 6187 with distance 183
path from 4568 to 6278 with distance 183
path from 5995 to 6278 with distance 181
path from 644 to 6188 with distance 181
path from 4193 to 6278 with distance 181
----------------------------------
Running time: 0.908267 s (~0 min)
----------------------------------
```

Program running to find the top 10 longest paths in the big graph file (`graph2.txt`) using serial operation (running dijkstra serially on every source vertex): 
```
----------------------------------
The top 10 longest paths: 
----------------------------------
path from 100201 to 36397 with distance 103
path from 100201 to 150246 with distance 103
path from 100201 to 92762 with distance 102
path from 100201 to 139117 with distance 102
path from 41354 to 36397 with distance 102
path from 41354 to 150246 with distance 102
path from 100201 to 36972 with distance 101
path from 100201 to 45436 with distance 101
path from 100201 to 133228 with distance 101
path from 46297 to 36397 with distance 101
----------------------------------
Running time: 6572.489423 s (~109 min)
----------------------------------
```

Program running to find the top 20 longest paths in the big graph file (`graph2.txt`) with OpenMP and dynamic scheduling (6 core Xeon E5-1650, max number of concurrent threads = 6), because there are multiple paths with the same distance, more than 10 are provided here:
```
----------------------------------
The top 20 longest paths: 
----------------------------------
path from 100201 to 36397 with distance 103
path from 100201 to 150246 with distance 103
path from 100201 to 92762 with distance 102
path from 100201 to 139117 with distance 102
path from 41354 to 36397 with distance 102
path from 41354 to 150246 with distance 102
path from 100201 to 36972 with distance 101
path from 100201 to 45436 with distance 101
path from 100201 to 133228 with distance 101
path from 46297 to 36397 with distance 101
path from 46297 to 150246 with distance 101
path from 41354 to 92762 with distance 101
path from 41354 to 139117 with distance 101
path from 40743 to 36397 with distance 101
path from 40743 to 150246 with distance 101
path from 26333 to 36397 with distance 101
path from 26333 to 150246 with distance 101
path from 100201 to 55279 with distance 100
path from 100201 to 105027 with distance 100
path from 46297 to 92762 with distance 100
----------------------------------
Running time: 1919.495081 s (~31 min)
----------------------------------
```

# Background

Write a C program that solves the all source shortest path problem by applying Dijkstra n = |V| times –– that is, once for each vertex in the input graph G, as described in the recorded lectures and outlined in the code snipped below. An important detail not described in typical Dijkstra pseudocode is the implementation of the next-closest vertex search. To get full credit, and to ensure the algorithm runs in reasonable time for the large input file provided, the search must be done using a binary min-heap priority queue*. Rather than save all of the path data, output only the 10 longest paths in a file with format:

start vertex 1, end vertex 1, distance 1<br>
start vertex 2, end vertex 2, distance 2<br>
...<br>
...<br>
start vertex 10, end vertex 10, distance 10

In addition to devising and running your own correctness tests, carry out a performance anaysis on the two graphs provided. No credit will be given for extremely inefficient implementations (several hours is the expected execution time in serial for large graph). To speed up execution of the large graph, OpenMP is strongly recommended, but not required for credit.

*An excellent reference for building a min-heap priority queue can be found here: https://bradfieldcs.com/algos/trees/priority-queues-with-binary-heaps/

*Dijkstra Shortest Path Main Loop*
```
while (processed[v] == false) {
    processed[v] = true;
    p = g->edges[v];
    while ( p != NULL) {
      w = p->y;
      weight = p->weight;
      if (distance[w] > (distance[v] + weight)) {
        distance[w] = (distance[v] + weight);
      }
      p = p->next;
    }
    /* find new min: naive approach, replace with min-heap priority queue */
    for (i=1,v=1,dist=INT_MAX; i<=g->nvertices; i++) {
      if ((processed[i] == false) && (dist > distance[i])) {
        dist = distance[i];
        v = i;
      }
    }
  }
```
