# 01204498 - Web Application Development

Chinnaporn Soonue
5610546231

============================================================

## Homework 3 - MPI

### FileName: Bubble_Sort.c

#### Compile

```
mpicc Bubble_Sort.c -o bubble
```

#### Run

```
mpiexec -np 2 ./bubble
```

**Output**
```
99999 99998 99997 99996 99995 99994 99993 99992 99991 99990
Array is NOT sorted
0 1 2 3 4 5 6 7 8 9
Array is sorted
Elapsed time MPI_Wtime is 12.292119
```

```
mpiexec -np 4 ./bubble
```

**Output**
```
99999 99998 99997 99996 99995 99994 99993 99992 99991 99990
Array is NOT sorted
0 1 2 3 4 5 6 7 8 9
Array is sorted
Elapsed time MPI_Wtime is 3.342011
```

```
mpiexec -np 10 ./bubble
```

**Output**
```
99999 99998 99997 99996 99995 99994 99993 99992 99991 99990
Array is NOT sorted
0 1 2 3 4 5 6 7 8 9
Array is sorted
Elapsed time MPI_Wtime is 1.185971
```

```
mpiexec -np 25 ./bubble
```

**Output**
```
99999 99998 99997 99996 99995 99994 99993 99992 99991 99990
Array is NOT sorted
0 1 2 3 4 5 6 7 8 9
Array is sorted
Elapsed time MPI_Wtime is 0.586941
```

============================================================
