#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000

void swap(int *xp, int *yp) {
  int temp = *xp;
  *xp = *yp;
  *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n) {
  int i, j;
  for (i = 0; i < n-1; i++)      
    for (j = 0; j < n-i-1; j++)
      if (arr[j] > arr[j+1])
	swap(&arr[j], &arr[j+1]);
}

int isSorted(int *a, int size) {
  int i;
  for (i = 0; i < size - 1; i++) {
    if (a[i] > a[i + 1]) {
      return 0;
    }
  }
  return 1;
}

// Function to print an array
void printArray(int arr[], int size)
{
  int i;
  for (i=0; i < size; i++)
    printf("%d ", arr[i]);
  printf("\n");
}

void merge(int m, int n, int A[], int B[], int C[]) {
  int i, j, k;
  i = 0;
  j = 0;
  k = 0;
  while (i < m && j < n) {
    if (A[i] <= B[j]) {
      C[k] = A[i];
      i++;
    } 
    else {
      C[k] = B[j];
      j++;
    }
    k++;
  }
  if (i < m) {
    for (int p = i; p < m; p++) {
      C[k] = A[p];
      k++;
    }
  } 
  else {
    for (int p = j; p < n; p++) {
      C[k] = B[p];
      k++;
    }
  }
}

int main(int argc, char** argv) {
	int i, n, n2, n4;
	int* A, *B, *C, *D;
	clock_t start, end;
	double elapsed_time, t1, t2;

	MPI_Init(NULL, NULL);
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	t1 = MPI_Wtime();
	A = (int *)malloc(sizeof(int)*N);
	B = (int *)malloc(sizeof(int)*N);
	C = (int *)malloc(sizeof(int)*N);
	D = (int *)malloc(sizeof(int)*N);
	if( rank == 0){
		if (A == NULL) {
			printf("Fail to malloc\n");
			exit(0);
		}
		for (i=N-1; i>=0; i--)
			A[N-1-i] = i;
		if( size == 2 ){
			MPI_Send(&A[N/2], N/2, MPI_INT, 1, 0, MPI_COMM_WORLD); //Token, Amount, Data type, receiver rank, tag, Socket
			printf("size = 2 Sending from rank %d \n",rank);
		}
		else if( size == 4 ){
			for(i = 1;i<=3;i++){
				MPI_Send(&A[(N/4)*i], N/4, MPI_INT, i, 0, MPI_COMM_WORLD); //Token, Amount, Data type, receiver rank, tag, Socket	
			printf("size = 4 Sending from rank %d \n",rank);
			}
		}
		else{
			printf("error");
			return 1;
		}
	if (isSorted(A, N))
	  printf("Array is sorted\n");
	else
	  printf("Array is NOT sorted\n");
	  
	}
	else{
		if( size == 2 ){
		printf("size = 2 Recv to %d \n", rank);
		MPI_Recv(A, N/2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else if( size == 4 ){
		printf("size = 4 Recv to %d \n", rank);
		MPI_Recv(A, N/4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	if( size == 2){
	bubbleSort(A, N/2);
	if(rank != 0)
	MPI_Send(A, N/2, MPI_INT, 0, 9, MPI_COMM_WORLD);
	printf("size 2 bub and send from %d \n",rank);
	}
	else if( size == 4){
	bubbleSort(A, N/4);
	if(rank != 0)
	MPI_Send(A, N/4, MPI_INT, 0, 9, MPI_COMM_WORLD);
	printf("size 4 bub and send from %d \n",rank);
	}
	if( rank == 0){
		if(size == 2){
		MPI_Recv(&A[N/2], N/2, MPI_INT, 1, 9, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		merge(N/2, N/2, A, &A[N/2], C);
		}
		else if( size == 4){
			for(i = 1; i<4;i++){
				MPI_Recv(&A[(N/4)*i], N/4, MPI_INT, i , 9, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			merge(N/4, N/4, A, &A[N/4], B);
			merge(N/4, N/4, &A[N/2], &A[N-(N/4)], D);
			merge(N/2, N/2, B, D, C);
		}
	printArray(&C[N-10], 10);
	
	if (isSorted(C, N))
	  printf("Array is sorted\n");
	else
	  printf("Array is NOT sorted\n");
	t2 = MPI_Wtime();
	printf( "Elapsed time MPI_Wtime is %f\n", t2 - t1 ); 
	}
	MPI_Finalize();
	return 0;
}