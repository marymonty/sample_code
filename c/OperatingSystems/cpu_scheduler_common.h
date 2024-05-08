#ifndef CPU_SCHEDULER_COMMON_H
  #define CPU_SCHEDULER_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

typedef struct {
  int pid;
  int arrival_time;
  double expected_runtime; // for tau
  int remaining_time; // for curr_cpu_burst or curr_io
  int priority;
  int num_bursts;
  int* cpu_burst_times;
  int* io_burst_times;
  int curr_cpu_burst;
  int curr_io_burst;
} process;

// Node
typedef struct node {
	process data;
	// Lower values indicate higher priority
	double priority;
	struct node* next;
} Node;

typedef struct {
  Node* processes;
  Node* readyQueue;
  Node* blockingQueue;
  process* running;
  int curr_runtime;
} scheduler;

// Function to Create A New Node
Node* newNode(process d, double p);
// Return the value at head
process peek(Node** head);

// Removes the element with the
// highest priority form the list
void pop(Node** head);

// Function to push according to priority
void push(Node** head, process d, double p);
// Function to check is list is empty
int isEmpty(Node** head);

double verifyRandomNum(double curr_rand, int upper_bound, double lambda);
double exponentialAvgFunc(double curr_rand, double lambda);
process* initProcesses(int num_processes, int upper_bound, int seed, double lambda);
scheduler* initScheduler(process* processes, int num_processes);

#endif