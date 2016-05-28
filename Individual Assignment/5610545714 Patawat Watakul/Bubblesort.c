#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000

void swap(int *v, int i, int j)
{
  int t;
  t = v[i];
  v[i] = v[j];
  v[j] = t;
}

void bubbleSort(int *v, int n)
{
  int i,j;
  for(i=n-2;i>=0;i--)
    for(j=0;j<=i;j++)
      if(v[j]>v[j+1])
        swap(v,j,j+1);
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

int * merge(int * v1, int n1, int * v2, int n2)
{
  int * result = (int *)malloc((n1 + n2) * sizeof(int));
  int i = 0;
  int j = 0;
  int k;
  for (k = 0; k < n1 + n2; k++) {
    if (i >= n1) {
      result[k] = v2[j];
      j++;
    }
    else if (j >= n2) {
      result[k] = v1[i];
      i++;
    }
    else if (v1[i] < v2[j]) {
      result[k] = v1[i];
      i++;
    }
    else {
      result[k] = v2[j];
      j++;
    }
  }
  return result;
}
// Function to print an array
void printArray(int arr[], int size)
{
  int i;
  for (i=0; i < size; i++)
    printf("%d ", arr[i]);
  printf("\n");
}

int main(int argc, char** argv) {
  MPI_Status status;
	int i, n;
	int* data, *temp;
	clock_t start, end;
	double elapsed_time, t1, t2;

//MPI
  MPI_Init(NULL, NULL);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

// Create data
	data = (int *)malloc(sizeof(int)*N);
	temp = (int *)malloc(sizeof(int)*N);
  if (world_rank == 0)
  {
	 if (data == NULL) {
		printf("Fail to malloc\n");
		exit(1);
	 }
	for (i=N-1; i>=0; i--)
		data[N-1-i] = i;
// Check sorted
    if (isSorted(data, N))
    printf("Array is sorted\n");
    else
    printf("Array is NOT sorted\n");
  }

	
//Sort Part
  MPI_Barrier(MPI_COMM_WORLD);
  
  t1 = MPI_Wtime();

  int s = N/world_size;
  MPI_Bcast(&s,1,MPI_INT,0,MPI_COMM_WORLD);
  int * chunk = (int *)malloc(s*sizeof(int));
  MPI_Scatter(data,s,MPI_INT,chunk,s,MPI_INT,0,MPI_COMM_WORLD);

  bubbleSort(chunk,s);

// merge
  int step = 1;
  while(step<world_size)
  {
    if(world_rank%(2*step)==0)
    {
      if(world_rank+step<world_size)
      {
        int m;
        MPI_Recv(&m,1,MPI_INT,world_rank+step,0,MPI_COMM_WORLD,&status);
        int * other;
        other = (int *)malloc(m*sizeof(int));
        MPI_Recv(other,m,MPI_INT,world_rank+step,0,MPI_COMM_WORLD,&status);
        chunk = merge(chunk,s,other,m);
        s = s+m;
      } 
    }
    else
    {
      int near = world_rank-step;
      MPI_Send(&s,1,MPI_INT,near,0,MPI_COMM_WORLD);
      MPI_Send(chunk,s,MPI_INT,near,0,MPI_COMM_WORLD);
      break;
    }
    step = step*2;
  }

  if(world_rank == 0){
    if (isSorted(chunk, N))
      printf("Array is sorted\n");
    else
      printf("%d : Array is NOT sorted\n", world_size); 
    t2 = MPI_Wtime();
    printf( "Elapsed time MPI_Wtime is %f\n", t2 - t1 );
  }

  MPI_Finalize();	
	return 0;
}