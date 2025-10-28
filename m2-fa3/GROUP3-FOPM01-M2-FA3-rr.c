
// program for rr

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// process struct
struct Process {
    int pid;
    int burst_time;
    int remaining_burst_time;
    int running_time;
    int arrival_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int start_time;
    int completion_time;
};

// function prototypes
void get_user_input(struct Process *processes, int *arr_len);
void round_robin(struct Process *processes, int arr_len, int quantum);
void sort(struct Process *processes, int arr_len);
void check_arrival(struct Process *processes, struct Process *ready_queue, int *pindex, int arr_len, int *ready_queue_len, int time);
void print_averages(struct Process *processes, int arr_len);
void print_header();
void print_process_stats(struct Process *processes, int arr_len);
void swap(struct Process *process_a, struct Process *process_b);

const int QUANTUM = 6;

int main() {
    int arr_len;
    printf("\nRound Robin\n");
    printf("Enter the number of processes: ");
    scanf("%d", &arr_len);


    // initialize the processes array
    struct Process *processes = malloc(sizeof(struct Process) * arr_len);

    // get user input for srtf
    get_user_input(processes, &arr_len);

    // run the rr algorithm
    round_robin(processes, arr_len, QUANTUM);
    print_header();
    print_process_stats(processes, arr_len);
    print_averages(processes, arr_len);


    // free the allocated memory for rr
    free(processes);

    return 0;
}


// prompt for inputs
void get_user_input(struct Process processes[], int *arr_len) {
    
    for (int i = 0; i < *arr_len; i++) {
        // get the pid
        printf("Enter the pid for process %d: ", i+1);
        scanf("%d", &processes[i].pid);

        // get the burst time and arrival times
        printf("Enter the burst time for process %d: ", i+1);
        scanf("%d", &processes[i].burst_time);

        // make the remaining burst time equal to the burst time since the rbt will just decrease
        processes[i].remaining_burst_time = processes[i].burst_time;
        processes[i].running_time = 0;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;

        // set the response time to -1 to show that it has not been computed yet
        processes[i].response_time = -1;
        processes[i].start_time = -1;

        printf("Enter the arrival time for process %d: ", i+1);
        scanf("%d", &processes[i].arrival_time);
    }
}   


void round_robin(struct Process processes[], int arr_len, int quantum) {
    // sort the processes by arrival time
    sort(processes, arr_len);

    // ready queue
    struct Process ready_queue[arr_len];
    int ready_queue_len = 0;

    // running process
    struct Process running_process = {0};

    // process index
    int pindex = 0;

    // current time
    int current_time = 0;
    int completed_processes = 0;
    int running_process_active = 0;


    while (completed_processes < arr_len) {
        // check if there is a process that arrives at current time
        check_arrival(processes, ready_queue, &pindex, arr_len, &ready_queue_len, current_time);

        // check for completion at current time
        if (running_process.pid != 0) {
            // if the process is completed, compute the statistics
            if (running_process.remaining_burst_time <= 0) {
                // compute the statistics
                running_process.completion_time = current_time;
                running_process.turnaround_time = running_process.completion_time - running_process.arrival_time;
                running_process.waiting_time = running_process.turnaround_time - running_process.burst_time;

                if (running_process.response_time == -1) {
                    running_process.response_time = running_process.start_time - running_process.arrival_time;
                }
                

                // assign the running process to the processes array
                for (int j = 0; j < arr_len; j++) {
                    if (processes[j].pid == running_process.pid) {
                        processes[j] = running_process;
                        break;
                    }
                }

                completed_processes++;
                running_process.running_time = 0;
                running_process_active = 0;

                // empty the running process
                struct Process null_process = {0};
                running_process = null_process;

            } else if (running_process.running_time >= quantum) {
                // if the process is not completed, check if it has reached the quantum

                // put the running process to the ready queue
                running_process.running_time = 0;
                ready_queue[ready_queue_len] = running_process;
                ready_queue_len++;

                // empty the running process
                struct Process null_process = {0};
                running_process = null_process;
                running_process_active = 0;
            }
        }

        // if the ready queue is not empty and there is no running process, put the process in the running process
        if (ready_queue_len > 0 && running_process_active == 0) {
            running_process = ready_queue[0];
            // reset the running time of the new running process
            running_process.running_time = 0;


            // if the running process has not started previously, set the start time   
            if (running_process.start_time == -1) {
                running_process.start_time = current_time;
            }

            // remove the process from the ready queue and move all elements to the left
            for (int i = 0; i < ready_queue_len-1; i++) {
                ready_queue[i] = ready_queue[i+1];
            }
            ready_queue_len--;
            running_process_active = 1;
        }

        if (running_process_active) {
            running_process.running_time += 1;
            running_process.remaining_burst_time -= 1;
            current_time += 1;
        } else if (pindex < arr_len) {
            current_time = processes[pindex].arrival_time;
        }
    }
}

void print_process_stats(struct Process processes[], int arr_len) {
    for (int i = 0; i < arr_len; i++) {
        printf("%-10d %-15d %-15d %-15d %-15d %-15d %-15d\n", processes[i].pid, processes[i].burst_time, processes[i].arrival_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time, processes[i].completion_time);
    }
}

// sort the processes by arrival time
void sort(struct Process processes[], int arr_len) {
    for (int i = 0; i < arr_len; i++) {
        for (int j = i+1; j < arr_len; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                swap(&processes[i], &processes[j]);
            }
        }
    }
}


// print average statistics
void print_averages(struct Process processes[], int arr_len) {
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int total_response_time = 0;
    for (int i = 0; i < arr_len; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
        total_response_time += processes[i].response_time;
    }
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting_time/arr_len);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time/arr_len);
    printf("Average Response Time: %.2f\n", (float)total_response_time/arr_len);
}


// helper function to check whether a process arrives at a certain time
void check_arrival(struct Process processes[], struct Process ready_queue[], int *pindex, int arr_len, int *ready_queue_len, int time) {
        while (*pindex < arr_len && processes[*pindex].arrival_time <= time) {
            ready_queue[*ready_queue_len] = processes[*pindex];
            *ready_queue_len += 1;
            *pindex += 1;
        }
}

void swap(struct Process *process_a, struct Process *process_b) {
    struct Process temp = *process_a;
    *process_a = *process_b;
    *process_b = temp;
}

// printing the header of the table
void print_header() {
    printf("\n");
    printf("%-10s %-15s %-15s %-15s %-15s %-15s %-15s\n", "PID", "Burst Time", "Arrival Time", "Waiting Time", "Turnaround Time", "Response Time", "Completion Time");
    printf("%-10s %-15s %-15s %-15s %-15s %-15s %-15s\n", "---", "----------", "------------", "------------", "-------------", "-------------", "-------------");
}

