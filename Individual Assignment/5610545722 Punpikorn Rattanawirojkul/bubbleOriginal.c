#include <stdio.h>
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
void printArray(int arr[], int size)
{
 int i;
 for (i=0; i < size; i++)
 printf("%d ", arr[i]);
 printf("\n");
}
int main() {
 int i, n;
 int* A;
 clock_t start, end;
 double elapsed_time;

 A = (int *)malloc(sizeof(int)*N);
 if (A == NULL) { 
 printf("Fail to malloc\n");
 exit(0);
 }
 for (i=N-1; i>=0; i--)
 A[N-1-i] = i;
 // print the last ten elements
 printArray(&A[N-10], 10);
 start = clock() ;
 bubbleSort(A, N);
 end = clock();
 // print the last ten elements
 printArray(&A[N-10], 10);
 elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
 printf("elapsed time = %lf\n", elapsed_time);
 return 0;
} 