# Homework 1 Submission

# Name: Bin Li

# Instruction

Code can be compile using command `gcc -pedantic -ansi -Wall -o <output_name> point.c listfunc.c -lm`. The output program takes one positive integer argument, indicating the number of data points to be used for the Array-List performance comparison. For example `./<output_name> 1000000` will store 1000000 points in array and list and compare their respective performance of iterating on these points. The program will first print the result of its unit test before printing the result of performance comparison. The test for the performance is to calculate the standard deviation of the distances (to the origin) of the stored points. This will require using the array/list to iterate over each point, calculate the distance, and add each distance to the sum and the sum of square. The performance is measured by the amount of computing time used for the test.

# Array vs. List Performance Comparison 

In my test, using Array took about 3.015409 seconds to calculate the standard deviation of 10000000 data, while using List took about 5.511500 seconds to calculate the standard deviation of 10000000 data. So Array is almost twice as fast as List on iterating.

# Design Discussion

In my design of the generic List, I strive to seperate the client-side (represented as the point.c) and the provider-side (represented as the listfunc.c) as much as possible. Client can create a List instance of any type of data provided that they need to supply it with the necessary functions to guarantee its full funtionality. These include `char * tostr(const void *, char *)`: function to stringfy the data, `int destory(void *)`: function to free the data, and `int compar (const void *, const void *)`: function to compare two data. These data are pass as void pointer so the clients need to cast them to their appropriate type when writing these function in their code. 

One of the weakness in my design might be the requirement for the stringfy function for printing list, `void list_print(List *list, char * (*tostr)(const void *, char *), int strsz)`. In the client-supply function `tostr()`, the first argument is the data pointer, and the second one is a pre-allocated character pointer. The function is supposed to fill the pre-allocated char pointer with using c function `sprintf` and return that char pointer. So clients need to trust that the provider of List will provide a big enough allocated buffer to fill. This is promised in the argument `int strsz`, which will be used in the `list_print` method sets the amount of memory needed for stringfy each stored data. `int strsz` is supplied by clients each time they call the `list_print` method. In general, this design seems a little bit convoluted, and the responsibilities of client and provider are not cleanly seperated.
cd .