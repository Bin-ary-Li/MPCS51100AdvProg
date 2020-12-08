# Homework-4

# Name: Bin Li

# Introduction

Run command `make array` to compile the program, and run it with `./array`. The details of the task is discussed in file [Task Description](hw4.pdf). 

# Program Output

For the ease of grading, the output of the program is also posted here. 

```
Verifying that the three methods are correctly implemented...

Finding the value of 5.0 in many different arrays...

 ------ Unionized array -------- 
- Indices of the query value in the corresponding arrays 
25277 76463 23989 157639 40878 5662 71964 46967 18498 16687 19604 3007 27172 5357 6556 18938 4969 11577 53949 11047 3204 39823 1345 29702 25746 35858 39530 26477 8772 17838 22056 9832 12842 1462 4869 15424 12379 12393 6802 559 741 1243 21244 29780 15583 11831 47124 25630 7550 9590 7924 12739 13741 10707 14078 14199 21592 20747 7135 4227 5439 32872 23124 14788 14780 783 25386 19433 
- Values stored at those indices of corresponding arrays 
5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 

 ------ Augmented arrays -------- 
- Indices of the query value in the corresponding arrays 
25277 76463 23989 157639 40878 5662 71964 46967 18498 16687 19604 3007 27172 5357 6556 18938 4969 11577 53949 11047 3204 39823 1345 29702 25746 35858 39530 26477 8772 17838 22056 9832 12842 1462 4869 15424 12379 12393 6802 559 741 1243 21244 29780 15583 11831 47124 25630 7550 9590 7924 12739 13741 10707 14078 14199 21592 20747 7135 4227 5439 32872 23124 14788 14780 783 25386 19433 
- Values stored at those indices of corresponding arrays 
5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 

 ------ Binary search -------- 
- Indices of the query value in the corresponding arrays 
25277 76463 23989 157639 40878 5662 71964 46967 18498 16687 19604 3007 27172 5357 6556 18938 4969 11577 53949 11047 3204 39823 1345 29702 25746 35858 39530 26477 8772 17838 22056 9832 12842 1462 4869 15424 12379 12393 6802 559 741 1243 21244 29780 15583 11831 47124 25630 7550 9590 7924 12739 13741 10707 14078 14199 21592 20747 7135 4227 5439 32872 23124 14788 14780 783 25386 19433 
- Values stored at those indices of corresponding arrays 
5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 

 ------- end of query --------

 ------- Please visually verify that the method outputs are consistent --------

Start performance comparison between the serial binary search (BS) on each array, search with the unionized array,
and with the augmented arrays.
---------------------------------------------------------
        Querying values in a large dataset
---------------------------------------------------------
 Num of Queries  |       Total Running Time (sec) 
---------------------------------------------------------
                       BS      |  Unionized |   Augmented 
---------------------------------------------------------
           10000 |   0.071806  |   0.003808 |    0.009315  
---------------------------------------------------------
          100000 |   0.716643  |   0.036946 |    0.088885  
---------------------------------------------------------
         1000000 |   7.128940  |   0.368455 |    0.882906  
---------------------------------------------------------
         2500000 |  17.834101  |   0.918593 |    2.205967  

-------------------------Build Time----------------------

  Unionized array: 1.379095, Augmented arrays: 0.148014   

-------------------------End of test---------------------
```

# Post-Deadline Update

The program is now updated to take an argument in case reviewer wants to use different values for testing consistency of methods. This is a quality of life update. The program is fully funtional before this update so it should not factor into the grading.

The program can now pass a floating point number as argument and it will search that value in the arrays provided in `arrays.txt`. 

For example by calling `./array 20.0`, the program will give the following output: 

```
Verifying that the three methods are correctly implemented...

Finding the value close to 20 in many different arrays...

 ------ Unionized array -------- 
- Indices of the query value in the corresponding arrays 
25439 76524 24096 157712 41034 5675 72097 47003 18543 16749 19685 3067 27219 5373 6574 19048 5021 11601 53984 11071 3268 39845 2708 29726 25769 35881 39553 26499 8795 17860 22079 9943 12874 2637 4892 15602 13995 13823 8205 589 933 3488 21333 30121 15773 12024 47282 25731 7598 9650 7984 12822 14848 11769 14938 15298 22723 20800 8104 4879 6077 33209 23463 15119 15108 1185 25478 19537 
- Values stored at those indices of corresponding arrays 
20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 

 ------ Augmented arrays -------- 
- Indices of the query value in the corresponding arrays 
25439 76524 24096 157712 41034 5675 72097 47003 18543 16749 19685 3067 27219 5373 6574 19048 5021 11601 53984 11071 3268 39845 2708 29726 25769 35881 39553 26499 8795 17860 22079 9943 12874 2637 4892 15602 13995 13823 8205 589 933 3488 21333 30121 15773 12024 47282 25731 7598 9650 7984 12822 14848 11769 14938 15298 22723 20800 8104 4879 6077 33209 23463 15119 15108 1185 25478 19537 
- Values stored at those indices of corresponding arrays 
20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 

 ------ Binary search -------- 
- Indices of the query value in the corresponding arrays 
25439 76524 24096 157712 41034 5675 72097 47003 18543 16749 19685 3067 27219 5373 6574 19048 5021 11601 53984 11071 3268 39845 2708 29726 25769 35881 39553 26499 8795 17860 22079 9943 12874 2637 4892 15602 13995 13823 8205 589 933 3488 21333 30121 15773 12024 47282 25731 7597 9650 7984 12822 14848 11769 14938 15298 22723 20800 8103 4878 6076 33209 23463 15119 15108 1185 25478 19537 
- Values stored at those indices of corresponding arrays 
20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 

 ------- end of query --------

 ------- Please visually verify that the method outputs are consistent --------
```
