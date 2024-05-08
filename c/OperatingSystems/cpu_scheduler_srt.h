/*
    the header file for the SRT algorithm
*/

#ifndef CPU_SCHEDULER_SRT_H
  #define CPU_SCHEDULER_SRT_H

#include "cpu_scheduler_common.h"


double calcSRTPriority(process current_proc, double lambda, double alpha);
void populateReadyQueue(scheduler* proc_scheduler, int current_time_elapsed, double lambda, double alpha);
int checkProcessState(process* proc);
void readyQueueToRunning(scheduler* proc_scheduler);
void checkRunning(scheduler* proc_scheduler);
void checkIOBurstStates(scheduler* proc_scheduler, int current_time_elapsed);
scheduler* runShortestRemainingTime(process* processes, int num_processes, double lambda, double alpha, int context_switch_time);

#endif