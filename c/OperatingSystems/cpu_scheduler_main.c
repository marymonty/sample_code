#include <time.h>

#include "cpu_scheduler_common.h"
//#include "cpu_scheduler_rr.h"
#include "cpu_scheduler_srt.h"

#define MAX_NUM_PROCS 26


int main(int argc, char** argv){
  int seed, upper_bound, num_processes;
  int context_switch_time, rr_time_slice;
  char* rr_queue_type;
  double lambda, alpha;

  seed = atoi(argv[1]);
  lambda = atof(argv[2]);
  upper_bound = atoi(argv[3]);
  num_processes = atoi(argv[4]);
  context_switch_time = atoi(argv[5]);
  alpha = atof(argv[6]);
  rr_time_slice = atoi(argv[7]);
  rr_queue_type = argv[8];

  process* processes = initProcesses(num_processes, upper_bound, seed, lambda);
  perror("AFTER INITPROCESSES");

  // scheduler* proc_scheduler = runRoundRobin(processes, num_processes, rr_queue_type, rr_time_slice, context_switch_time);
  //
  //
  // while(!isEmpty(&proc_scheduler->processes)){
  //   p = peek(&proc_scheduler->processes);
  //   printf("Pid: %d, arrival_time: %f, priority: %f\n", p.pid, p.arrival_time, proc_scheduler->processes->priority );
  //   pop(&proc_scheduler->processes);
  //
  // }

  //runFCFS(processes, num_processes, context_switch_time);

  runShortestRemainingTime(processes, num_processes, lambda, alpha, context_switch_time);



  return EXIT_SUCCESS;
}