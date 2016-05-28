#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define N 100000

struct arg_struct {
	int *p;
	int n;
};

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
void bubbleSort(void *args) {
	struct arg_struct *arguments = (struct args_struct*)args;
	int i, j, n = arguments->n;
	int* arr = arguments->p;
	for (i = 0; i < n-1; i++) {
		for (j = 0; j < n-i-1; j++) {
			if (arr[j] > arr[j+1]) {
				swap(&arr[j], &arr[j+1]);
			}
		}
	}
}
void merge(int arr[], int a[], int b[], int aSize, int bSize) {
	int i = 0, j = 0;
	int aV = aSize;
	int bV = bSize;
	while (i < aSize + bSize) {
		if (aV > 0 && bV > 0) { //both is available
			if (a[0] < b[0]) { //pick a
				arr[i++] = a[0];
				a = &a[1];
				aV--;
			} else if (a[0] == b[0]) { //pick both
				arr[i++] = a[0];
				arr[i++] = b[0];
				a = &a[1];
				b = &b[1];
				aV--;
				bV--;
			} else { //pick b
				arr[i++] = b[0];
				b = &b[1];
				bV--;
			}
		} else {
			//one is available
			if (aV > 0) { //pick a
				arr[i++] = a[0];
				a = &a[1];
				aV--;
			} else if (bV > 0) { //pick b
				arr[i++] = b[0];
				b = &b[1];
				bV--;
			} else { //nothing left
				break;
			}
		}
	}
}
int main() {
	int i, n;
	int *A;
	clock_t start, end;
	double elapsed_time;
	A = (int *)malloc(sizeof(int)*N);
	if (A == NULL) { 
		printf("Fail to malloc\n");
		exit(0);
	}
	for (i=N-1; i>=0; i--)
		A[N-1-i] = i;

	int half = N/2;
	int* first = &A[0];
	int* second = &A[half];

	pthread_t t1, t2;

	// 1st argument
	struct arg_struct *args1 = (struct arg_struct*)malloc(sizeof(struct arg_struct*));
	args1->p = malloc(sizeof(int)*half);
	args1->p = first;
	args1->n = half;

	// 2nd argument
	struct arg_struct *args2 = (struct arg_struct*)malloc(sizeof(struct arg_struct*));
	args2->p = malloc(sizeof(int)*half);
	args2->p = second;
	args2->n = half;

	// print the last ten elements
	printArray(&A[N-10], 10);

	start = clock();
	////////////////////////////////
	pthread_create(&t1, NULL, (void*)bubbleSort, (void*)args1);
	pthread_create(&t2, NULL, (void*)bubbleSort, (void*)args2);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	int *temp = &A[half];
	// merge
	int *p;
	p = (int*)malloc(sizeof(int)*N);
	merge(p, &A[0], &A[half], half, half);
	///////////////////////////////
	end = clock();

	// print the last ten elements
	printArray(&p[N-10], 10);

	elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
	printf("elapsed time = %lf\n", elapsed_time);
	return 0;
} 