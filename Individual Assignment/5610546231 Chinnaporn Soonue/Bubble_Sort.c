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

void printArray(int arr[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int *swap_big(int tmp[], int size) {
    int* A = (int *)malloc(sizeof(int)*N);
    for(int i=0; i<size; i++){
        for(int j=0; j<N/size; j++){
            A[((N/size) * (size - i - 1)) + j] = tmp[((N/size) * i) + j];
        }
    }
    return A;
}

int main(int argc, char** argv) {
    int i, n;
    int* A;
    clock_t start, end;
    double elapsed_time, t1, t2;

    MPI_Init(NULL, NULL);
    t1 = MPI_Wtime();
    MPI_Status status;

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    A = (int *)malloc(sizeof(int)*N);
    if (A == NULL) {
        printf("Fail to malloc\n");
        exit(0);
    }

    for (i=N-1; i>=0; i--)
        A[N-1-i] = i;

    if(world_rank == 0){
        printArray(&A[0], 10);
        if (isSorted(A, N))
            printf("Array is sorted\n");
        else
            printf("Array is NOT sorted\n");

        for (int i = 1; i < world_size; ++i) {
            MPI_Send(&A[(N/world_size) * i], N/world_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        bubbleSort(&A[0], N/world_size);
    } else {
        MPI_Recv(&A[(N/world_size) * world_rank], N/world_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        bubbleSort(&A[(N/world_size) * world_rank], N/world_size);
        MPI_Send(&A[(N/world_size) * world_rank], N/world_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if(world_rank == 0) {
        for (int i = 1; i < world_size; ++i) {
            MPI_Recv(&A[(N/world_size) * (i)], N/world_size, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }
        A = swap_big(&A[0], world_size);
        printArray(&A[0], 10);
        if (isSorted(A, N))
            printf("Array is sorted\n");
        else
            printf("Array is NOT sorted\n");
        t2 = MPI_Wtime();
        printf( "Elapsed time MPI_Wtime is %f\n", t2 - t1 );
    }
    MPI_Finalize();
    return 0;
}

