#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define N 100000

void printArray(int arr[], int size)
{
	int i;
	for (i=0; i < size; i++)
		printf("%d ", arr[i]);
	printf("\n");
}

void swap(int *xp, int *yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

void bubbleSort(int arr[], int n) {
	int i, j;
	for (i = 0; i < n-1; i++) {
		for (j = 0; j < n-i-1; j++) {
			if (arr[j] > arr[j+1]) {
				// printf("swap %d and %d\n", arr[j], arr[j+1]);
				swap(&arr[j], &arr[j+1]);
			}
		}
	}
}

int* merge(int a[], int b[], int aSize, int bSize) {
	int *result = malloc(sizeof(int*)*(aSize + bSize));
	int i = 0, j = 0;
	int aV = aSize;
	int bV = bSize;
	while (i < aSize + bSize) {
		if (aV > 0 && bV > 0) { //both is available
			if (a[0] < b[0]) { //pick a
				result[i++] = a[0];
				a = &a[1];
				aV--;
			} else if (a[0] == b[0]) { //pick both
				result[i++] = a[0];
				result[i++] = b[0];
				a = &a[1];
				b = &b[1];
				aV--;
				bV--;
			} else { //pick b
				result[i++] = b[0];
				b = &b[1];
				bV--;
			}
		} else {
			//one is available
			if (aV > 0) { //pick a
				result[i++] = a[0];
				a = &a[1];
				aV--;
			} else if (bV > 0) { //pick b
				result[i++] = b[0];
				b = &b[1];
				bV--;
			} else { //nothing left
				break;
			}
		}
	}
	return result;
}

int main() {
	MPI_Init(NULL, NULL);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int i, n;
	MPI_Status status;
	int *A, *piece[world_size];
	int *buffer;
	int numOfPiece = N/world_size;
	clock_t start, end;
	double elapsed_time;
	int *result;
	result = (int*)malloc(sizeof(int)*N);

	A = (int *)malloc(sizeof(int)*N);
	if (A == NULL) { 
		printf("Fail to malloc\n");
		exit(0);
	}
	for (i=N-1; i>=0; i--)
		A[N-1-i] = i;

	//Setup the pointers
	for (i = 0; i < world_size; i++) {
		piece[i] = (int*)malloc(sizeof(int)*numOfPiece);
		piece[i] = &A[i*numOfPiece];
	}

	//buffer hold temp value
	buffer = (int*)malloc(sizeof(int)*N);

	if (world_rank == 0) {

		printArray(&A[N-10], 10);
		start = clock();
		///////////////////////////////////
		bubbleSort(piece[world_rank], numOfPiece);

		int recvTime = world_size/2;
		int numAmount = 0;
		while (recvTime > 0) {
			MPI_Recv(buffer, N, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
             &status);
			MPI_Get_count(&status, MPI_INT, &numAmount);
			printf("receive from %d, with size of %d\n", status.MPI_SOURCE, numAmount);
			//First time
			if (recvTime == world_size/2) {	
				result = merge(piece[0], buffer, numAmount, numAmount);	
			} else {
				result = merge(result, buffer, numAmount, numAmount);
			}
			recvTime--;
		}
		///////////////////////////////////
		end = clock();
		elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
		printf("elapsed time = %lf\n", elapsed_time);
		//Last Print
		printf("\nFrom %d\n", world_rank);
		printArray(&result[N-10], 10);
		printf("\n");
	} 

	if (world_rank != 0) {
		//Sorting
		bubbleSort(piece[world_rank], numOfPiece);
	}

	if (world_rank % 2 == 1) {
		MPI_Send(piece[world_rank], numOfPiece, MPI_INT, world_rank-1, 0,
           MPI_COMM_WORLD);
	}

	if (world_rank % 2 == 0 && world_rank != 0) {
		MPI_Recv(piece[world_rank+1], N, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
		result = merge(piece[world_rank], piece[world_rank+1], numOfPiece, numOfPiece);
		if (world_rank == 2) {
			MPI_Send(result, numOfPiece*2, MPI_INT, 0, 0,
           MPI_COMM_WORLD);
		}
	}
	
	MPI_Finalize();
} 