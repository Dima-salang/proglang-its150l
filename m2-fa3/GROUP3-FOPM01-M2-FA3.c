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
};

// function prototypes
void get_user_input(struct Process processes[], int *arr_len);
void round_robin(struct Process processes[], int arr_len, int quantum);
void srtf(struct Process processes[], int arr_len);
void sort(struct Process processes[], int arr_len);
int sum_burst_time(struct Process processes[], int arr_len);
int check_arrival(struct Process processes[], int arr_len, int time);
void print_averages(struct Process processes[], int arr_len);
void print_header();
void print_process_stats(struct Process processes[], int arr_len);
struct Process get_min_remaining_time(struct Process processes[], int *arr_len);
void insert(struct Process processes[], int *arr_len, struct Process process);
void min_heapify(struct Process processes[], int arr_len, int index);
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

    for (int i = 0; i < arr_len; i++) {
        printf("%-10d %-15d %-15d %-15d %-15d %-15d\n", processes[i].pid, processes[i].burst_time, processes[i].arrival_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time);
    }

    // free the allocated memory for rr
    free(processes);

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

    // print the processes
    for (int i = 0; i < arr_len; i++) {
        printf("%-10d %-15d %-15d %-15d %-15d %-15d\n", processes[i].pid, processes[i].burst_time, processes[i].arrival_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time);
    }


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

    // current time
    int current_time = 1;

    // sum of burst time for time to run the rr
    int burst_time_sum = sum_burst_time(processes, arr_len);

    for (int i = 0; i < burst_time_sum; i++) {
        // check if there is a process that arrives at current time
        int pindex = check_arrival(processes, arr_len, current_time);
        if (pindex != -1) {
            // check if there is a running process
            if (running_process.pid != 0) {
                // put the new process in the ready queue
                ready_queue[ready_queue_len] = processes[pindex];
                ready_queue_len++;
            } else {
                // put the new process in the running process
                running_process = processes[pindex];
                // reset the running time
                running_process.running_time = 0;
            }
        }

        // check for completion at current time
        if (running_process.pid != 0) {
            // if the process is completed, compute the statistics
            if (running_process.remaining_burst_time <= 0) {
                // compute the statistics
                running_process.turnaround_time = current_time - running_process.arrival_time;
                running_process.waiting_time = running_process.turnaround_time - running_process.burst_time;
                running_process.response_time = running_process.waiting_time;
                
                // print the running process
                printf("%d %d %d %d %d %d %d %d\n", running_process.pid, running_process.burst_time, running_process.arrival_time, running_process.waiting_time, running_process.turnaround_time, running_process.response_time, running_process.running_time, running_process.remaining_burst_time);

                // assign the running process to the processes array
                processes[i] = running_process;

                // empty the running process
                struct Process null_process = {0};
                running_process = null_process;

            } else if (running_process.running_time >= quantum) {
                // if the process is not completed, check if it has reached the quantum

                // put the running process to the ready queue
                ready_queue[ready_queue_len] = running_process;
                ready_queue_len++;

                // empty the running process
                struct Process null_process = {0};
                running_process = null_process;
            }else {
                // if the process neither completed nor reached the quantum, increment the running time
                running_process.running_time++;
            }
            running_process.remaining_burst_time--;
        }

        // if the ready queue is not empty and there is no running process, put the process in the running process
        if (ready_queue_len > 0 && running_process.pid == 0) {
            running_process = ready_queue[0];
            // remove the process from the ready queue and move all elements to the left
            for (int i = 0; i < ready_queue_len-1; i++) {
                ready_queue[i] = ready_queue[i+1];
            }
            ready_queue_len--;
        }
        current_time++;
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
    int current_time = 1;

    // sum of burst time for time to run the rr
    int burst_time_sum = sum_burst_time(processes, arr_len);

    for (int i = 0; i < burst_time_sum; i++) {
        // check if there is a process that arrives at current time
        int pindex = check_arrival(processes, arr_len, current_time);
        if (pindex != -1) {
            // check if there is a running process and that the running process has a smaller rbt
            if (running_process.pid != 0) {
                if (running_process.remaining_burst_time < processes[pindex].remaining_burst_time) {
                    // put the new process in the ready queue
                    insert(ready_queue, &ready_queue_len, processes[pindex]);
                } else {
                    // put the running process to the ready queue
                    insert(ready_queue, &ready_queue_len, running_process);
                    
                    // put the new process in the running process
                    running_process = processes[pindex];
                }
            } else {
                // put the new process in the running since there is no running process
                running_process = processes[pindex];
            }
        }

        // check for completion at current time
        if (running_process.pid != 0) {
            if (running_process.remaining_burst_time <= 0) {
                // compute statistics
                running_process.turnaround_time = current_time - running_process.arrival_time;
                running_process.waiting_time = running_process.turnaround_time - running_process.burst_time;
                running_process.response_time = running_process.waiting_time;

                // assign back to the processes array
                processes[i] = running_process;

                // empty the running process
                struct Process null_process = {0};
                running_process = null_process;
            }
            // decrement the remaining burst time
            else {
                running_process.remaining_burst_time--;
            }
        }

        // check if there is a process in the ready queue and no running process
        if (ready_queue_len > 0 && running_process.pid == 0) {
            running_process = get_min_remaining_time(ready_queue, &ready_queue_len);
        }
        current_time++;
    }
}


void print_process_stats(struct Process processes[], int arr_len) {
    for (int i = 0; i < arr_len; i++) {
        printf("%-10d %-15d %-15d %-15d %-15d %-15d\n", processes[i].pid, processes[i].burst_time, processes[i].arrival_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time);
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


// sum the burst time of all processes to calculate the length of the gantt chart
int sum_burst_time(struct Process processes[], int arr_len) {
    int sum = 0;
    for (int i = 0; i < arr_len; i++) {
        sum += processes[i].burst_time;
    }
    return sum;
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
int check_arrival(struct Process processes[], int arr_len, int time) {
    for (int i = 0; i < arr_len; i++) {
        if (processes[i].arrival_time == time) {
            return i;
        }
    }
    return -1;
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
    printf("%-10s %-15s %-15s %-15s %-15s %-15s\n", "PID", "Burst Time", "Arrival Time", "Waiting Time", "Turnaround Time", "Response Time");
    printf("%-10s %-15s %-15s %-15s %-15s %-15s\n", "---", "----------", "------------", "------------", "-------------", "-------------");
}

