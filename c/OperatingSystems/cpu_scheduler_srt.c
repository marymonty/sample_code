/*
  SRT : SHORTEST REMAINING TIME 
    The SRT algorithm is a preemptive version of the SJF algorithm 
        (SJF ALGORITHM: processes are stored in the ready queue in order of priority based on their CPU burst times.
            More specifically, the process with the shortest CPU burst time will be selected as the next process
                executed by the CPU.) 
    In SRT, when a process arrives, before it enters the ready queue, if it has a CPU burst time that is 
    less than the remaining time of the currently running process, a preemption occurs. When such a 
    preemption occurs, the currently running process is added back to the ready queue. 
*/

#include "cpu_scheduler_srt.h"




/* calcSRTPriority
    calculate the priority for SRT
    params:
        current_proc : the current process we are getting the priority of
*/
double calcSRTPriority(process current_proc, double lambda, double alpha)
{
    printf("\t\tIN calcSRTPriority\n");
    /* the priority should be based off the remaining time, remaining time is initially set to 0, then 
        set from the current cpu burst time of the process.
       have a check for if the remaining time is not 0, then use the remaining time, else use curr_cpu_burst time */
    double tau; //= current_proc.cpu_burst_time[curr_cpu_burst];
    if (current_proc.expected_runtime == 0)
    {
        /* initial tau = 1/lambda */
        tau = (1/lambda);
        printf("\t\tFIRST TAU = %f\n", tau);
        current_proc.expected_runtime = tau;
        printf("\t\tSET PROCESS %d EXPECTED RUNTIME TO BE: %f\n", current_proc.pid, current_proc.expected_runtime);
        return tau - (1.0/.9);
    }
    else
    {
        /* tau(i+1) = (alpha * t(i)) + ((1-alpha) * tau(i)) */
        tau = (alpha * current_proc.cpu_burst_times[current_proc.curr_cpu_burst]) + ((1-alpha) * current_proc.expected_runtime);
        printf("\t\tCURRENT TAU = %f\n", tau);
        current_proc.expected_runtime = tau;
        printf("\t\tSET PROCESS %d EXPECTED RUNTIME TO BE: %f\n", current_proc.pid, current_proc.expected_runtime);
        return tau - (1.0/current_proc.pid);
    }
}


/* populateReadyQueue
    a function to populate the readyQueue based on the priority set in the 
    calcSRTPriority function
    params:
        proc_scheduler : the schedule of processes, made in order of arrival times
        current_time_elapsed : the time that has passed in the simulation
*/
void populateReadyQueue(scheduler* proc_scheduler, int current_time_elapsed, double lambda, double alpha)
{
    printf("\tIN POPULATE READY QUEUE\n");

    /* first, get the current process by using peek */
    process current_proc = peek(&proc_scheduler->processes);
    /* next get the arrival time */
    int current_proc_arrival = current_proc.arrival_time;
    
    printf("\tPROCESS %d HAS ARRIVAL TIME OF %d\n", current_proc.pid, current_proc.arrival_time);

    /* priority_metric is how we order the processes in the readyQueue */
    double current_proc_priority;
    /* check the current processes arrival time against the current_time_elapsed */
    while (current_time_elapsed <= current_proc_arrival)
    {
        printf("\tADDING PROCESS %d TO THE READY QUEUE\n", current_proc.pid);

        /* calculate the priority metric for the current process */
        current_proc_priority = calcSRTPriority(current_proc, lambda, alpha);
        /* if the readyQueue is empty we need to make a newNode to head the list */
        if(isEmpty(&proc_scheduler->readyQueue))
        {
            printf("\tTHIS IS THE FIRST PROCESS BEING ADDED TO THE READYQUEUE\n");

            proc_scheduler->readyQueue = newNode(current_proc, current_proc_priority);
        }
        /* else the readyQueue already has a head and we can just push to it */
        else
        {
            push(&proc_scheduler->readyQueue, current_proc, current_proc_priority);
        }
        /* pop from the list since we have moved the process from the processes  
            to the readyQueue so it no longer needs to be in the processes list */
        pop(&proc_scheduler->processes);
        /* need to update the current process and arrival time for the while loop 
            (we dont need to go to &proc_scheduler->processes->next because when we pop
            we reload the processes into the head */
        if (!isEmpty(&proc_scheduler->processes))
        {
            current_proc = peek(&proc_scheduler->processes);
            current_proc_arrival = current_proc.arrival_time;
            printf("\tPROCESS %d HAS ARRIVAL TIME OF %d\n", current_proc.pid, current_proc.arrival_time);
        }
        else    //no more processes
        {
            return;
        }
    }
}


/*
    runShortestRemainingTime
    running the SRT algorithm
    parameters:
        processes: the linked list of processes
        num_processes: the number of processes
        lambda: needed for the estimations of cpu burst times
        alpha: also needed for the estimations of the cpu burst times
        context_switch_time: the measure of the context switch time 
*/
scheduler* runShortestRemainingTime(process* processes, int num_processes, double lambda, double alpha, int context_switch_time)
{
    /* created the counter for the time elapsed, needed to organize arrival times and what can be added to the ready queue */
    int current_time_elapsed, i;
    current_time_elapsed = 0;
   
    /* get the initial schedule of all processes (this will be based off of arrival time only with tie breaks going to lower pids) */
    scheduler* proc_scheduler = initScheduler(processes, num_processes);

    /* initialize the running process */
    proc_scheduler->running = calloc(1, sizeof(process));


    /* while loop, should keep running until all processes are finished */
    while (!isEmpty(&proc_scheduler->readyQueue) || proc_scheduler->running != NULL 
            || proc_scheduler->blockingQueue != NULL || !isEmpty(&proc_scheduler->processes)) 
    {
        /* first lets populate the readyQueue with the processes */
        if (!isEmpty(&proc_scheduler->processes))
        {
            populateReadyQueue(proc_scheduler, current_time_elapsed, lambda, alpha);

        }
        //print the readyQueue
        printf("PRINTING READY QUEUE\n");
        process p = peek(&proc_scheduler->readyQueue);
        while(&proc_scheduler->readyQueue->next)
        {
            printf("\tprocess id: %d  arrival time: %d   expected runtime: %f   \n", p.pid, p.arrival_time, p.expected_runtime);
            p = peek(&proc_scheduler->readyQueue->next);
        }


        printf("do i get out of there???\n");

        /* check if nothing is in the readyQueue (say nothing arrives at time 0, there is nothing else 
            to do but continue to the next time), to check this make sure the readyQueue, running, and blockingQueue
            are empty / NULL 
        if (isEmpty(&proc_scheduler->readyQueue) && proc_scheduler->running == NULL && proc_scheduler->blockingQueue == NULL)  
        {
            printf("AT TIME %d NOTHING IS IN THE READYQUEUE, RUNNING, AND BLOCKINGQUEUE\n", current_time_elapsed);
            continue;
        }
        readyQueueToRunning(proc_scheduler);
        */

        /* add one to the current_time_elapsed */
        current_time_elapsed++;
    }
    return proc_scheduler;
}


/* checkProcessState
        a process that checks if the process is done running
        params: 
            proc : the current running process

int checkProcessState(process* proc)
{
    int proc_state = 0;
    // i dont think this should be proc->remaining_time 
    if (proc.remaining_time == 0)
    {
        proc_state = 1;
    }
    return proc_state;
}
*/


/*
void checkBlockingQueue()
{
     check if any of the io_bursts are complete to move back to the readyQueue
        there will never be an io_burst without another cpu_burst after it (there is one less io_burst than cpu_burst) 
      
}
*/


/* readyQueueToRunning
    check if the readyQueue is not empty 
        if readyQueue is not empty:
            check if there is a running process
                if there is no running process:
                    peek and pop the first process in the readyQueue and set it to the running process
                else there is a running process:
                    peek at first process in readyQueue to check the remaining time
                    if the remaining time is less than the running process remaining time:
                        there is a preemption, current running process gets sent back to readyQueue, reset the running process with new process
                    else the new remaining time is greater than the running process remaining time:
                        let the current running process keep running  
void readyQueueToRunning(scheduler* proc_scheduler)
{
    printf("\tIN readyQueueToRunning\n");

    if (!isEmpty(&proc_scheduler->readyQueue))
    {
        printf("THE READYQUEUE IS NOT EMPTY\n");

        if (proc_scheduler->running == NULL)
        {
            printf("THE RUNNING PROCESS IS NULL (NO RUNNING PROCESS)\n");
            
            //process* proc = (process*)calloc(1, sizeof(process));
            process new_proc = peek(&proc_scheduler->readyQueue);
            //proc = &new_proc;
            // set the process's remaining time 
            proc->remaining_time = proc->cpu_burst_time[proc->curr_cpu_burst];
            // should this be running[0] or just running 
            proc_scheduler->running[0] = new_proc;
            pop(&proc_scheduler->readyQueue);
        }
        else    // there is a running process 
        {
            printf("THERE IS A PROCESS RUNNING, PROCESS %d\n", proc_scheduler->running.pid);
            
            process new_proc = peek(&proc_scheduler->readyQueue);
            new_proc.remaining_time = new_proc.cpu_burst_time[new_proc.curr_cpu_burst];
            
            printf("CURRENT RUNNING PROCESS HAS %d REMAINING TIME, NEW PROCESS HAS %d\n", proc_scheduler->running.remaining_time, new_proc.remaining_time);
            
            // check if there needs to be a preemption 
            if (proc_scheduler->running.remaining_time > new_proc.remaining_time)
            {
                printf("PREEMPTION\n");
                process old_proc = proc_scheduler->running;
                proc_scheduler->running[0] = new_proc;
                pop(&proc_scheduler->readyQueue);
                double old_proc_priority = (old_proc.remaining_time - 1/(old_proc.pid));
                push(&proc_scheduler->readyQueue, old_proc, old_proc_priority);
            }
        }
    }    
}
*/


/*
void checkRunning(scheduler* proc_scheduler)
{
    printf("\tIN checkRunning\n");

    if (&proc_scheduler->running != NULL)
    {
        // check if the process is finished running 
        process current_proc = proc_scheduler->running;

        printf("\tTHERE IS A RUNNING PROCESS WITH PID %d", current_proc.pid);

        if (current_proc.remaining_time == 0)
        {
            current_proc.curr_cpu_burst += 1;
            // figure out tau estimation and store it in expected runtime
            //    tau is used for          ???            
            //    tau(i+1) = (alpha * t(i)) + ((1-alpha) * tau(i))
            current_proc.expected_runtime = ()
            current_proc.remaining_time = current_proc.io_burst_times[curr_io_burst];

        }
        // else process is not finished running 
        else
        {
            current_proc.remaining_time -= 1;
        }
    }
}
*/


/*
void checkIOBurstStates(scheduler* proc_scheduler, int current_time_elapsed)
{
    printf("\t IN checkIOBurstStates\n");

    process current_proc;
    // if the blockingQueue is empty, we don't have to do anything 
    if (!isEmpty(&proc_scheduler->blockingQueue))
    {
        // trying to go through the blockingQueue to see if any bursts are completed
        //    decrement each burst 
        while(proc_scheduler->blockingQueue->next)
        {
            current_proc = peek(&proc_scheduler->blockingQueue);
            
            printf("\t attempting to go through the blockingQueue, on pid %d\n", current_proc.pid);

            if (current_proc.remaining_time == 0)
            {
                // need to pop it from the blockingQueue and put it back in the readyQueue 

            }
        }
    }
}
*/
