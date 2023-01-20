#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define NUM_CHILD 5

typedef struct {
	int num1;
	int num2;
} addition;

// recursive adding function
void* add(void* v) {
	addition *a = (addition*)v;
	if ((*a).num2 == 1) {
		return (void*)(1 + (*a).num1);		
	}

	(*a).num2 -= 1;
	void *ret1 = 1 + add((void*)a);
	return ret1;
}


int main() {
	pthread_t children[NUM_CHILD * (NUM_CHILD-1) + 1];
	children[NUM_CHILD * (NUM_CHILD-1) + 1] = '\0';
	addition * addition_arr[NUM_CHILD * (NUM_CHILD-1) + 1];
	addition_arr[NUM_CHILD * (NUM_CHILD-1) + 1] = '\0';

	for (int i = 0; i < NUM_CHILD-1; i++) {
		for (int j = 1; j <= NUM_CHILD; j++) {
			pthread_t tid;
			printf("Main starting thread add() for [%d + %d]\n", i+1, j);
			addition *a;
			a = calloc(1, sizeof(addition));
			(*a).num1 = i+1;
			(*a).num2 = j;
			addition_arr[(i*NUM_CHILD) + j] = a;
			addition *a_copy;
			a_copy = calloc(1, sizeof(addition));
			(*a_copy).num1 = i+1;
			(*a_copy).num2 = j;
			int val = pthread_create(&tid, NULL, add, (void*)a_copy);

			if (val != 0) {
				/*error*/
				fprintf(stderr, "Could not create pthread\n");
				return -1;
			}
			else {
				/*success*/
				children[(i*NUM_CHILD) + j] = tid;
				printf("Thread %li starting thread add() for [%d + %d]\n", tid, i+1, j);
			}
		}
	}
	/*rejoin the children*/
	for (int m = 0; m < NUM_CHILD-1; m++) {
		for (int n = 1; n <= NUM_CHILD; n++) {
			int sum;
			pthread_join(children[(m*NUM_CHILD) + n], (void**)&sum);
			addition * new_a = addition_arr[(m*NUM_CHILD) + n];
			printf("In main, collecting thread %li computed [%d + %d] = %d\n", children[(m*NUM_CHILD) + n], (*new_a).num1, (*new_a).num2, sum);
			free(addition_arr[(m*NUM_CHILD) + n]);
		}
	}
}
