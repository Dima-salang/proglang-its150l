
// program for srtf and rr

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
void srtf(struct Process *processes, int arr_len);
void sort(struct Process *processes, int arr_len);
void check_arrival(struct Process *processes, struct Process *ready_queue, int *pindex, int arr_len, int *ready_queue_len, int time);
void print_averages(struct Process *processes, int arr_len);
void print_header();
void print_process_stats(struct Process *processes, int arr_len);
struct Process get_min_remaining_time(struct Process *processes, int *arr_len);
int peek_min_remaining_time(struct Process *processes);
void insert(struct Process *processes, int *arr_len, struct Process process);
void min_heapify(struct Process *processes, int arr_len, int index);
void swap(struct Process *process_a, struct Process *process_b);

const int QUANTUM = 6;

int main() {
    int arr_len;

    // initialize the processes array
    struct Process *processes = malloc(sizeof(struct Process) * arr_len);

    // get user input for srtf
    get_user_input(processes, &arr_len);

    printf("\nShortest Remaining Time First\n");
    printf("Enter the number of processes: ");
    scanf("%d", &arr_len);

    // initialize the processes array
    processes = malloc(sizeof(struct Process) * arr_len);

    // get user input for srtf
    get_user_input(processes, &arr_len);

    // build the min-heap from the processes
    for (int i = arr_len/2 - 1; i >= 0; i--) {
        min_heapify(processes, arr_len, i);
    }

    // run the srtf algorithm
    srtf(processes, arr_len);
    print_header();
    print_process_stats(processes, arr_len);
    print_averages(processes, arr_len);

    // free the allocated memory for srtf
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


void srtf(struct Process processes[], int arr_len) {
    // sort the processes by arrival time
    sort(processes, arr_len);


    // ready queue
    struct Process ready_queue[arr_len];
    int ready_queue_len = 0;

    // running process
    struct Process running_process = {0};

    // current time
    int current_time = 0;
    int running_process_active = 0;
    int completed_processes = 0;

    // process index
    int pindex = 0;

    while (completed_processes < arr_len) {
        // check if there is a process that arrives at current time
        check_arrival(processes, ready_queue, &pindex, arr_len, &ready_queue_len, current_time);

        // check for completion at current time
        if (running_process_active) {
            if (running_process.remaining_burst_time <= 0) {
                // compute statistics
                running_process.completion_time = current_time;
                running_process.turnaround_time = running_process.completion_time - running_process.arrival_time;
                running_process.waiting_time = running_process.turnaround_time - running_process.burst_time;
                

                // assign back to the processes array
                for (int j = 0; j < arr_len; j++) {
                    if (processes[j].pid == running_process.pid) {
                        processes[j] = running_process;
                        break;
                    }
                }

                completed_processes++;
                running_process_active = 0;
                running_process.running_time = 0;

                // empty the running process
                struct Process null_process = {0};
                running_process = null_process;
            }

            // check if there is a running process and that the running process has a smaller rbt
            if (running_process.remaining_burst_time > peek_min_remaining_time(ready_queue)) {
                // insert the running process back to the ready queue since the incoming process rbt is less
                insert(ready_queue, &ready_queue_len, running_process);

                // get the process with the smallest rbt
                running_process = get_min_remaining_time(ready_queue, &ready_queue_len);
                running_process_active = 1;
                running_process.running_time = 0;

                if (running_process.start_time == -1) {
                    running_process.start_time = current_time;
                    running_process.response_time = running_process.start_time - running_process.arrival_time;
                }
            }
        }

        // check if there is a process in the ready queue and no running process
        if (ready_queue_len > 0 && running_process_active== 0) {
            running_process = get_min_remaining_time(ready_queue, &ready_queue_len);
            running_process_active = 1;
            running_process.running_time = 0;

            if (running_process.start_time == -1) {
                running_process.start_time = current_time;
                running_process.response_time = running_process.start_time - running_process.arrival_time;
            }
        }

        if (running_process_active) {
            running_process.running_time += 1;
            running_process.remaining_burst_time -= 1;
        }
        current_time += 1;
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
        insert(ready_queue, ready_queue_len, processes[*pindex]);
        *pindex += 1;
    }
}



// MIN-HEAP FUNCTIONS

// get the min from the heap and remove it
struct Process get_min_remaining_time(struct Process processes[], int *arr_len) {
    // get the head
    struct Process min = processes[0];

    // remove the head by moving the last element to the head
    processes[0] = processes[*arr_len-1];
    *arr_len -= 1;

    // min_heapify
    min_heapify(processes, *arr_len, 0);

    return min;
}

int peek_min_remaining_time(struct Process processes[]) {
    return processes[0].remaining_burst_time;
}

void insert(struct Process processes[], int *arr_len, struct Process process) {
    // insert the process into the min heap
    processes[*arr_len] = process;
    *arr_len += 1;
    
    int index = *arr_len -1;
    while (index > 0 && processes[(index-1)/2].remaining_burst_time > processes[index].remaining_burst_time) {
        // swap the processes
        swap(&processes[index], &processes[(index-1)/2]);

        // update the index
        index = (index-1)/2;
    }
}
    
void min_heapify(struct Process processes[], int arr_len, int index) {
    int left = 2*index + 1;
    int right = 2*index + 2;
    int smallest = index;
    if (left < arr_len && processes[left].remaining_burst_time < processes[smallest].remaining_burst_time) {
        smallest = left;
    }
    if (right < arr_len && processes[right].remaining_burst_time < processes[smallest].remaining_burst_time) {
        smallest = right;
    }
    if (smallest != index) {
        // swap the processes
        swap(&processes[index], &processes[smallest]);
        // min_heapify the rest of the heap
        min_heapify(processes, arr_len, smallest);
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

