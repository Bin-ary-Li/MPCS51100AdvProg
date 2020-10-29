# Homework 3: N-body Simulation

![n-body simulation](/images/cover.png)

# Name

Bin Li

# Description

In this code, a serial, two-dimensional version of the Barnes-Hut (BH) n-body algorithm is implemented. The accuracy and the performance of the algorithm are also discussed and explored. 

# Instruction

Run command `make` will compile the program with name `nbody`. There are three optional argument `./nbody number_of_particle number_of_iteration whether_to_run_speed_analysis`, the first one takes a positive integer and sets the number of particles to simulate, the second also takes a positive integer and sets the number of iteration to run, the third one, if specified, indicates whether to run performance speed comparison between different algorithms used for nbody simulation. The third one needs to be one of the `-speed_analysis_full` or `-speed_analysis_short`, the latter test the performance on smaller data size so is faster to run. 

Simulation output is generated in datafile `./nbody.csv` which is necessary for generating animated simulation. 

To generate animated simulation, run the c program to generate the datafile first and then run the python script using command `python nbody_sim_anim.py`. A few pre-render example of animation can be found in folder `./simulation_animation`.

Example:

`./nbody`: default to 200 particles and 200 iterations, not running speed analysis.

`./nbody 500 20`: Run a 500 particles simulation for 20 iterations, not running speed analysis. 

`./nbody 500 20 -speed_analysis_short`: Run a 500 particles simulation for 20 iterations, and a short version of speed analysis. 

**Note**:
The program slows down on higher particle count or iteration number. The maximum is num_particle\*num_iteration <= 3000\*200. 

## Background 

In the classical algorithm for simulating a n-body system, gravitational force on every particle from every particle in the system is calculated, resulting in O(N^2) time complexity. On the other hand, the Barnes-Hut algorithm, also known as the tree code for n-body simulation, builds an octree (or a quadtree for two-dimensional system) to store particles in the leaf nodes and group them in internal nodes. The tree can then be used to perform calculation by reaching to each particle in the tree, or approximating the influence of a group of particles by their center of mass, which is represented in internal nodes. Utilizing the structural property of tree, Barnes-Hut results in a better time complexity of O(NlogN). 

## Design Details

### Tree-building

This code builds a quadtree to implement the Barnes-Hut algorithm in two-dimensional system. 

Tree-building is verified by examining the print-out tree node via breadth first search `void qTree_print_bfs(QTNode * root)`. 

### Domain size and out-of-bound particle

In BH, building quadtree requires specifying the size of domain to split on. At first this is limited to 20.0, which gives x dimension and y dimension the range of (-10,10). As the system updating at each iteration, the position of a particle might be moved outside of the range. 

Solution to out-of-bound particle is explored. I have tried to wrap the position of particle around a range, and what go beyond one side come out from the other side, effectively treating the topology of the system as a torus. But the sudden change of particle position from warping seems to purturb the system, introducing extra uncertainty and making later verification more difficult. 

The current solution is to resize the domain of the quadtree at each iteration to contain the farthermost particle. For example, if the original size of the domain is 2.0, from -1.0 to 1.0 on each dimension, say a particle moves to (1.5, 0.5), then in the next iteration the size of domain will expand to 3.0, from -1.5 to 1.5. 

## Measure and Result

### Accuracy measure

The accuracy of a BH quadtree with distance threshold of 1.0 is measured by comparing the output positions with those of the classical algorithm and a BH quadtree with distance threshold of 0.0, which degenerates into the classical algorithm. As the degenerated BH tree is effectively perform the same calculation as the classical algorithm, we should expect they would have the same output. 

Mismatch points in the outputs are counted for each iteration. In the end the average number of mismatch per iteration is calculated and printed. 

A snippet of the code print-out: 
```
---------Accuracy Measure------------
Baseline: On average 13.092000 error per iteration in 1000 iterations of 20-body simulation.
Barnes-Hut Approximation: On average 12.316000 error per iteration in 1000 iterations of 20-body simulation (distance threshold: 1.000000). 
There are 14.723000 mismatched particles per iteration between Tree code with distance threshold 1.000000 and Tree code with threshold 0 (i.e. degenerating into N^2 algorithm).
```

The error data at each iteration is also generated each time and recorded in `sim_error.csv` file. 

It is hard to measure the correctness of a nbody simulation. A naive direct comparison between the outputs of the classical algorithm and Barnes-Hut will work in theory, but in practice this is complicated by the determinism (or rather the indeterminism) of floating-point arithmetic. 

To combat that, an error tolerance of 0.01 is set and scale with the number of iteration, and max out at 5.0. The two particles is moved to the same position if their absolute differences in x and y coordinate are both less than the tolerance level. (This is effectively L1 distance. It is used because it is slightly more lenient than L2.) This means that, for example, in the second iteration, p1 at (1.110, 1.0) and p2 (1.129, 1.0) are at the same position (1.129 - 1.110 = 0.019 < 2*0.01). 

Even with the tolerance, the accumulation of round-off error is still very prominent, especially with more particles and/or more runs of iteration. A careful examination finds that for each particle, the degenrated tree code calculates the gravitational influence from all particles in an order different from that of the classical algorithm, which use the index order of for loop. This will cause round-off issue as floating-point arithmetic is not guarantee to be associative. 

~~To remedy that, the order of calculation of the degenerated tree is recorded for each particle, and later enforced on the classical algorithm. This solution greatly improves the consistency, if not fully resolve the round-off issue. Before enforcing the order, the number of mismatches between the outputs of the degenerated tree and the classical algorithm starts to increase at as early as the 3rd iteration and escalates so fast that no particle will be matched by the 100th iteration. After enforcing the order, the mismatch only starts to appears at the 10th iteration (depending on the number of simulated particles), increases at a steady pace, and staying at around 50% of the total number of particles even at the later part of the simulation (> 1000 iterations).~~ 

The strategy of enforcing the order of computation might still hold anyway, but in my further testing it seems to have inconsistent impact on the result. Another look at the code reveals that this might be more complicated than I originally thought. Not surprisingly the precision also has to do with the amount of computation performed (especially floating-point addition/subtraction). During particles initialization, if there are too many of them pack in a small domain, say randomly generating 1000 particles in the space bounded from (-1, 1). The amount of gravitational force that moves each particle will be large relative to their starting position, magnifying the effect of numerical error. If we generate 1000 particles in a domain range from (-1000, 1000), there will be more space between particles so they will receive less gravitational influence and move less. 

Indeed, in my testing, the number of mismatches I get (and how early I start to get) in 500 iterations of 500-body simulation scales with the size of the domain where I randomly initialize the particle. This might give some clues about how to avoid the exponential growth of numerical error in the testing. In general, we might not want to initialize too many particles in a very small domain, which will magnify even the smallest inaccuracy in the floating-point arithmetic. We also might not want too large a space because if the particles are too far off from each other the whole system can be stationary. 

### Performance measure

Performance of the classical algorithm, a serial BH tree code, and a parallel tree code are also measured and compared. 

A snipper of code print-out (parallel with 6-thread): 
```
---------------------------------------------------------
        Simulating 20-iteration n-body system
---------------------------------------------------------
Particle Count|                 Running Time 
---------------------------------------------------------
                  Classical | Tree Code  | Parallel Tree 
---------------------------------------------------------
         4000 |   3.808127  |   0.053323 |    0.211322  
---------------------------------------------------------
         8000 |  15.211811  |   1.067958 |    0.514685  
---------------------------------------------------------
        12000 |  34.296299  |   1.562325 |    0.748733  
---------------------------------------------------------
        18000 |  77.178832  |   2.286734 |    1.184785  
---------------------------------------------------------
        30000 | 214.521535  |   3.655020 |    2.074201  
-------------------------End of test---------------------
```

## Weakness and Possible Improvements

Floating-point determinism is still an outstanding issue for the correctness measurement of this implementation. A deeper understanding of floating-point arithmetic is required to be able discerning other potentially inconsistency-causing details in the code. But a self-gravitating system of point particles like n-body simulation is, by its nature, extremely sensitive to numerical errors. In the end it might be a fool's errand to try to come up with a deterministic solution to n-body integration. 

The reliability of N-body simulation can also be evaluated by studying certain statistical properties of such physical systems. But as of now I do not possess the necessary expertise in this domain to give any more insight in that regard. 

~~Another line of thinking is to verify the implementation graphically, by plotting out the system at each timestep or even animating it. This verification can be more intuitive than direct comparison and potentially requires less work than addressing the issue of numerical error.~~ (**implemented**) 

# References

Barnes–Hut simulation
https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation

OpenMP Case study: The Barnes-Hut N-body Algorithm SMM
https://www.cs.unc.edu/~prins/Classes/633/Slides/09-smm4.pdf

The kinds of errors made in N-body simulations
http://www.cs.toronto.edu/~wayne/research/thesis/msc/node5.html

Quinlan, Gerald D., and Scott Tremaine. "On the reliability of gravitational N-body integrations." Monthly Notices of the Royal Astronomical Society 259, no. 3 (1992): 505-518.
