# MPCS 51100 HW4
**Name**: Phoebe Collins, UCID: 12277438

## References
I referenced Geeksforgeeks in implementing binarySort and mergeKArrays. I have included the specific links as a comment in the implementation of these two functions.

## Discussion on program and performance
I did not observe any compilation or run-time errors. The Valgrind report is clean. 

Number of iterations in lookup kernel: 100000

Structure 1             - avgTime: 0.0000033703   totTime: 0.337027 

Structure 2             - avgTime: 0.0000004682   totTime: 0.046817 

Structure 3             - avgTime: 0.0000012404   totTime: 0.124035 

Structure 4(10350 bins) - avgTime: 0.0000008628   totTime: 0.086277 

I implemented the function find_optimal_bucket to determine the optimal number of buckets between 1 and x to minimize search time over y iterations. Given an x of 15000 and y of 1000, optimal number of buckets was 10350 buckets.

## Compiling and running
`make main` and then `./main`. Finally, run `make clean`.
