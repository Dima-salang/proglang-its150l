
// program for fcfs

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// process struct
struct process {
    int pid;
    int burst_time;
    int arrival_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
};

// function prototypes
void fcfs(struct process processes[], int arr_len);
void get_user_input(struct process processes[], int *arr_len);
void sort(struct process processes[], int arr_len);
void print_stats(struct process processes[], int arr_len);
void swap(struct process *process_a, struct process *process_b);
void print_header();
void print_averages(struct process processes[], int arr_len);
void draw_gantt_chart(struct process processes[], int arr_len, int sum_burst_time, int algo);
int sum_burst_time(struct process processes[], int arr_len);
int check_arrival(struct process processes[], int arr_len, int time);

int main() {
    int arr_len;

    printf("\nFirst-Come First-Serve\n");
    printf("Enter the number of processes: ");
    scanf("%d", &arr_len);

    // initialize the processes array
    struct process *processes = malloc(sizeof(struct process) * arr_len);

    // get user input for fcfs
    get_user_input(processes, &arr_len);

    // run fcfs algo
    fcfs(processes, arr_len);
    print_stats(processes, arr_len);
    print_averages(processes, arr_len);
    draw_gantt_chart(processes, arr_len, sum_burst_time(processes, arr_len), 0);

    // free the allocated memory for fcfs
    free(processes);

    return 0;
}

// prompt for inputs
void get_user_input(struct process processes[], int *arr_len) {
    for (int i = 0; i < *arr_len; i++) {
        // get the pid
        printf("Enter the pid for process %d: ", i+1);
        scanf("%d", &processes[i].pid);

        // get the burst time and arrival times
        printf("Enter the burst time for process %d: ", i+1);
        scanf("%d", &processes[i].burst_time);

        // set the arrival time to 0
        processes[i].arrival_time = i;
    }
}   

// first-come first-serve function
void fcfs(struct process processes[], int arr_len) {
    // sort the processes by arrival time
    sort(processes, arr_len);
    printf("\nFirst-Come First-Serve\n");
    int current_time = 0;
    for (int i = 0; i < arr_len; i++) {

        // simulate the processing
        current_time += processes[i].burst_time;

        // calculate the turnaround time
        processes[i].turnaround_time = current_time - processes[i].arrival_time;

        // calculate the waiting time
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;

        // calculate response time
        processes[i].response_time = processes[i].waiting_time;

    }
    
}

// sort the processes by arrival time
void sort(struct process processes[], int arr_len) {
    for (int i = 0; i < arr_len; i++) {
        for (int j = i+1; j < arr_len; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                swap(&processes[i], &processes[j]);
            }
        }
    }
}

void print_stats(struct process processes[], int arr_len) {
    print_header();
    for (int i = 0; i < arr_len; i++) {
        printf("%-10d %-15d %-15d %-15d %-15d %-15d\n", processes[i].pid, processes[i].burst_time, processes[i].arrival_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time);
    }
}


// print average statistics
void print_averages(struct process processes[], int arr_len) {
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



// draw gantt chart
// algo is:
// 0 - fcfs
// 1 - sjf
void draw_gantt_chart(struct process processes[], int arr_len, int sum_burst_time, int algo) {
    // simulate the processing per second
    int completed_processes = 0;

    // process index
    int pindex = -1;

    // ready queue for processes that arrive
    struct process ready_queue[arr_len];
    int ready_queue_len = 0;

    // the currently running process
    struct process running_process = {0};

    // buffer time just for drawing the gantt chart
    int buffer_time = 5;

    printf("\nGantt Chart\n");

    // print legend
    printf("\nLegend:\n");
    printf("PID <time>: CPU is processing\n");
    printf("* <time>: CPU is idle\n\n");
    int current_time = 0;
    while (completed_processes < arr_len) {
        // check if there is a process that arrives at current time
        pindex = check_arrival(processes, arr_len, current_time);
        if (pindex != -1) {
            // check if there is a running process
            if (running_process.pid != 0) {
                // put the new process in the ready queue
                    ready_queue[ready_queue_len] = processes[pindex];
                    ready_queue_len++;
            } else {
                // put the new process in the running process
                running_process = processes[pindex];
            }
        }
        // check for completion at curr time. and decrement the burst time
        if (running_process.pid != 0) {
            if (running_process.burst_time <= 0) {
                struct process null_process = {0};
                running_process = null_process;
                completed_processes++;
            }
            running_process.burst_time--;
        }
        // check if there is a process in the ready queue and no running process
        // if so, put the process in the running process
        if (ready_queue_len > 0 && running_process.pid == 0) {
            // put the process in the running process
                running_process = ready_queue[0];
                // remove the process from the ready queue
                for (int i = 0; i < ready_queue_len-1; i++) {
                    ready_queue[i] = ready_queue[i+1];
                }
                ready_queue_len--;
        }

        // print the gantt chart
        if (running_process.pid != 0) {
            // print the process and the time
            printf("| P%d %d ", running_process.pid, current_time);
        } else {
            printf("| * %d ", current_time);
        }

        // increment the time
        current_time++;
    }
    printf("|");
    printf("\n");
}


// helper function to check whether a process arrives at a certain time
int check_arrival(struct process processes[], int arr_len, int time) {
    for (int i = 0; i < arr_len; i++) {
        if (processes[i].arrival_time == time) {
            return i;
        }
    }
    return -1;
}

// sum the burst time of all processes to calculate the length of the gantt chart
int sum_burst_time(struct process processes[], int arr_len) {
    int sum = 0;
    for (int i = 0; i < arr_len; i++) {
        sum += processes[i].burst_time;
    }
    return sum;
}

void swap(struct process *process_a, struct process *process_b) {
    struct process temp = *process_a;
    *process_a = *process_b;
    *process_b = temp;
}

// printing the header of the table
void print_header() {
    printf("\n");
    printf("%-10s %-15s %-15s %-15s %-15s %-15s\n", "PID", "Burst Time", "Arrival Time", "Waiting Time", "Turnaround Time", "Response Time");
    printf("%-10s %-15s %-15s %-15s %-15s %-15s\n", "---", "----------", "------------", "------------", "-------------", "-------------");
}
