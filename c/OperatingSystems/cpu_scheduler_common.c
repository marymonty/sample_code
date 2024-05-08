#include "cpu_scheduler_common.h"

double verifyRandomNum(double curr_rand, int upper_bound, double lambda){
  while (curr_rand > upper_bound){
    curr_rand = drand48();
    curr_rand = exponentialAvgFunc(curr_rand, lambda);
  }
  return curr_rand;
}
double exponentialAvgFunc(double curr_rand, double lambda){
  // printf("Curr_rand: %f\n",curr_rand );
  // printf("Lambda: %f\n", lambda );
  // printf("log(curr_rand): %f\n", log(curr_rand));
  return (-1*log(curr_rand)) / lambda;
}
process* initProcesses(int num_processes, int upper_bound, int seed, double lambda){
  int pid, cpu_burst;
  double curr_rand;
  // fprintf(stdout, "WE GET HERE\n");
  //Init processes array
  process* procs = (process*)calloc(sizeof(process), num_processes);
  //Init rand environement
  srand48(seed);
  for (pid = 0; pid < num_processes; pid++){
    //Set pid
    procs[pid].pid = pid;
    //Set arrival_time
    curr_rand = drand48();
    curr_rand = exponentialAvgFunc(curr_rand, lambda);
    // printf("Curr_rand: %f \n", curr_rand );
    curr_rand = verifyRandomNum(curr_rand, upper_bound, lambda);
    // printf("pid: %d, arrival_time: %f\n", pid, curr_rand );
    // fprintf(stdout, "pid: %d\n", pid);

    procs[pid].arrival_time = floor(curr_rand);
    printf("ARR_TIME:%d \n", floor(curr_rand));
    //Set num of cpu bursts
    curr_rand = drand48();
    curr_rand = exponentialAvgFunc(curr_rand, lambda);
    curr_rand = verifyRandomNum(curr_rand, upper_bound, lambda);

    procs[pid].num_bursts = trunc((curr_rand*100)) + 1;

    //Init cpu_burst_times && io_burst_times
    procs[pid].cpu_burst_times = (int*)calloc(sizeof(int), procs[pid].num_bursts);
    procs[pid].io_burst_times = (int*)calloc(sizeof(int), procs[pid].num_bursts - 1);

    for (cpu_burst = 0; cpu_burst < procs[pid].num_bursts; cpu_burst++){
      //Generate burst time
      curr_rand = drand48();
      curr_rand = exponentialAvgFunc(curr_rand, lambda);
      curr_rand = verifyRandomNum(curr_rand, upper_bound, lambda);

      procs[pid].cpu_burst_times[cpu_burst] = ceil(curr_rand);
      //Dont generate an io_burst_time for the last cpu_burst
      if (cpu_burst < procs[pid].num_bursts - 1){
        //Generate io time
        curr_rand = drand48();
        curr_rand = exponentialAvgFunc(curr_rand, lambda);
        curr_rand = verifyRandomNum(curr_rand, upper_bound, lambda);

        procs[pid].io_burst_times[cpu_burst] = ceil(curr_rand);
      }
    }
    procs[pid].curr_cpu_burst = 0;
    procs[pid].curr_io_burst = 0;
    procs[pid].remaining_time = 0;
  }
  return procs;
}

scheduler* initScheduler(process* processes, int num_processes){
  perror("TOP of initScheduler");
  int i;
  double priority_metric;
  scheduler* proc_scheduler = (scheduler*)calloc(sizeof(scheduler), 1);

  if (num_processes > 1){
    for (i = 0; i < num_processes; i++){
      priority_metric =  processes[i].arrival_time - 1.0/processes[i].pid;
      //If first process init linked list
      if (i == 0){
        //Special case cannot divide by pid == 0
        priority_metric = processes[i].arrival_time - 1.0/.9;
        proc_scheduler->processes = newNode(processes[0], priority_metric);
        printf("Added pid: %d with priority: %f and arr_time: %d\n", processes[i].pid, priority_metric, processes[i].arrival_time);
        continue;
      }
      push(&proc_scheduler->processes, processes[i], priority_metric);
      printf("Added pid: %d with priority: %f and arr_time: %d\n", processes[i].pid, priority_metric, processes[i].arrival_time);
    }
  }
  //
  proc_scheduler->readyQueue = NULL;
  proc_scheduler->blockingQueue = NULL;
  proc_scheduler->running = NULL;
  perror("END OF INITSCHEDULER");
  return proc_scheduler;
}

// Function to Create A New Node
Node* newNode(process d, double p){
	Node* temp = (Node*)malloc(sizeof(Node));
	temp->data = d;
	temp->priority = p;
	temp->next = NULL;

	return temp;
}
// Return the value at head
process peek(Node** head){
	return (*head)->data;
}

// Removes the element with the
// highest priority form the list
void pop(Node** head) {
	Node* temp = *head;
	(*head) = (*head)->next;
	free(temp);
}

// Function to push according to priority
void push(Node** head, process d, double p) {
	Node* start = (*head);
	Node* temp = newNode(d, p);
	if ((*head)->priority > p) {
		temp->next = *head;
		(*head) = temp;
	}
	else {
		while (start->next != NULL &&
			start->next->priority < p) {
			start = start->next;
		}
		temp->next = start->next;
		start->next = temp;
	}
}

// Function to check is list is empty
int isEmpty(Node** head) {
	return (*head) == NULL;
}