# Homework 1 Submission

# Name: Bin Li

# Instruction

Code can be compile using command `gcc -pedantic -ansi -Wall -o <output_name> point.c listfunc.c -lm`. The output program takes one positive integer argument, indicating the number of data points to be used for the Array-List performance comparison. For example `./<output_name> 1000000` will store 1000000 points in array and list and compare their respective performance of iterating on these points. The program will first print the result of its unit test before printing the result of performance comparison. The test for the performance is to calculate the standard deviation of the distances (to the origin) of the stored points. This will require using the array/list to iterate over each point, calculate the distance, and add each distance to the sum and the sum of square. The performance is measured by the average amount of computing time used in 20 trials of the test.

# Array vs. List Performance Comparison 

In my test, using Array took about 0.303456 seconds to calculate the standard deviation of 1000000 data, while using List took about 7.728617 seconds to calculate the standard deviation of 1000000 data. So Array seems to be much faster than List on this test.

# Design Discussion

In my design of the generic List, I strive to seperate the client-side (represented as the point.c) and the provider-side (represented as the listfunc.c) as much as possible. Client can create a List instance of any type of data provided that they need to supply it with the necessary functions to guarantee its full funtionality. These include `char * tostr(const void *, char *)`: function to stringfy the data, `int destory(void *)`: function to free the data, and `int compar (const void *, const void *)`: function to compare two data. These data are pass as void pointer so the clients need to cast them to their appropriate type when writing these function in their code. 

One of the weakness in my design might be the requirement for the stringfy function for printing list, `void list_print(List *list)`. When client supplies the function `tostr(const void *, char *)`, it needs to be taking the data to be stringfy as the first argument, and a pre-allocated character pointer as the second argument. The function is supposed to fill the pre-allocated char pointer using `sprintf` in the c standard library and return that char pointer. So when client is writing that function, he/she needs to trust that the provider will provide a big enough allocated buffer to fill. This is promised in the argument `int strsz` passed into the List constructor, which will be used in the `list_print` method sets the amount of memory needed for stringfying each stored data. In general, this design might be a little bit convoluted, and requires some extra level of communication and agreement between the client and the provider.