// program for fcfs and sjf

#include <stdio.h>
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
void sjf(struct process processes[], int arr_len);
void insert(struct process processes[], int *arr_len, struct process process);
void min_heapify(struct process processes[], int arr_len, int index);
void swap(struct process *process_a, struct process *process_b);
void print_header();
void print_averages(struct process processes[], int arr_len);
void draw_gantt_chart(struct process processes[], int arr_len, int sum_burst_time, int algo);
int check_arrival(struct process processes[], int arr_len, int time);
int sum_burst_time(struct process processes[], int arr_len);
void get_user_input(struct process processes[], int *arr_len);
struct process get_min_burst_time(struct process processes[], int *arr_len);

int main() {
    int arr_len;

    // initialize the processes array with initial length
    struct process processes[10];

    // get user input for fcfs
    get_user_input(processes, &arr_len);

    // run fcfs algo
    fcfs(processes, arr_len);
    print_averages(processes, arr_len);
    draw_gantt_chart(processes, arr_len, sum_burst_time(processes, arr_len), 0);

    // get user input for sjf
    get_user_input(processes, &arr_len);

    // build the min-heap from the processes
    for (int i = arr_len/2 - 1; i >= 0; i--) {
        min_heapify(processes, arr_len, i);
    }

    // run sjf algo
    sjf(processes, arr_len);
    print_averages(processes, arr_len);
    draw_gantt_chart(processes, arr_len, sum_burst_time(processes, arr_len), 1);

    
    return 0;
    
}

// prompt for inputs
void get_user_input(struct process processes[], int *arr_len) {
    printf("Enter the number of processes: ");
    scanf("%d", arr_len);
    
    for (int i = 0; i < *arr_len; i++) {
        printf("Enter the burst time for process %d: ", i+1);
        scanf("%d", &processes[i].burst_time);
        processes[i].arrival_time = i+1;
        processes[i].pid = i+1;
    }
}   

// first-come first-serve function
void fcfs(struct process processes[], int arr_len) {
    printf("\nFirst-Come First-Serve\n");
    int current_time = 1;
    print_header();
    for (int i = 0; i < arr_len; i++) {
        // calculate the waiting time
        processes[i].waiting_time = current_time - processes[i].arrival_time;

        // simulate the processing
        current_time += processes[i].burst_time;

        // calculate the turnaround time
        processes[i].turnaround_time = current_time - processes[i].arrival_time;

        // calculate response time
        processes[i].response_time = processes[i].waiting_time;

        // print the process
        printf("%-10d %-15d %-15d %-15d %-15d %-15d\n", processes[i].pid, processes[i].burst_time, processes[i].arrival_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time);
    }
    
}


// shortest job first function
void sjf(struct process processes[], int arr_len) {
    printf("\nShortest Job First\n");
    int current_time = 1;

    // copy the processes array
    struct process processes_copy[arr_len];
    for (int i = 0; i < arr_len; i++) {
        processes_copy[i] = processes[i];
    }
    int arr_len_copy = arr_len;

    print_header();
    for (int i = 0; i < arr_len; i++) {
        // get the process with the min burst time
        struct process min_process = get_min_burst_time(processes_copy, &arr_len_copy);
        
        
        // calculate the waiting time
        min_process.waiting_time = current_time - min_process.arrival_time;

        // simulate the processing
        current_time += min_process.burst_time;

        // calculate the turnaround time
        min_process.turnaround_time = current_time - min_process.arrival_time;

        // calculate response time
        min_process.response_time = min_process.waiting_time;

        // assign the min process to the processes array
        processes[i] = min_process;

        // print the process
        printf("%-10d %-15d %-15d %-15d %-15d %-15d\n", min_process.pid, min_process.burst_time, min_process.arrival_time, min_process.waiting_time, min_process.turnaround_time, min_process.response_time);
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
    printf("PID: CPU is processing\n");
    printf("* : CPU is idle\n");
    int current_time = 1;
    while (current_time <= sum_burst_time+buffer_time) {
        // check if there is a process that arrives at current time
        pindex = check_arrival(processes, arr_len, current_time);
        if (pindex != -1) {
            // check if there is a running process
            if (running_process.pid != 0) {
                // put the new process in the ready queue
                if (algo == 0) {
                    ready_queue[ready_queue_len] = processes[pindex];
                    ready_queue_len++;
                } 
                // if it is sjf, we insert the process in the min-heap by treating the ready queue as a min-heap
                else if (algo == 1) {
                    insert(ready_queue, &ready_queue_len, processes[pindex]);
                }
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
            }
            running_process.burst_time--;
        }
        // check if there is a process in the ready queue and no running process
        // if so, put the process in the running process
        if (ready_queue_len > 0 && running_process.pid == 0) {
            // put the process in the running process
            if (algo == 0) {
                running_process = ready_queue[0];
                // remove the process from the ready queue
                for (int i = 0; i < ready_queue_len-1; i++) {
                    ready_queue[i] = ready_queue[i+1];
                }
                ready_queue_len--;
            } 
            // if it is sjf, we get the min process from the min-heap and remove it
            else if (algo == 1) {
                running_process = get_min_burst_time(ready_queue, &ready_queue_len);
            }
        }

        // print the gantt chart
        if (running_process.pid != 0) {
            printf("| P%d ", running_process.pid);
        } else {
            printf("| * ");
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

int sum_burst_time(struct process processes[], int arr_len) {
    int sum = 0;
    for (int i = 0; i < arr_len; i++) {
        sum += processes[i].burst_time;
    }
    return sum;
}


// MIN-HEAP FUNCTIONS

// get the min from the heap and remove it
struct process get_min_burst_time(struct process processes[], int *arr_len) {
    // get the head
    struct process min = processes[0];

    // remove the head by moving the last element to the head
    processes[0] = processes[*arr_len-1];
    *arr_len -= 1;

    // min_heapify
    min_heapify(processes, *arr_len, 0);

    return min;
}

void insert(struct process processes[], int *arr_len, struct process process) {
    // insert the process into the min heap
    processes[*arr_len] = process;
    *arr_len += 1;
    
    int index = *arr_len -1;
    while (index > 0 && processes[(index-1)/2].burst_time > processes[index].burst_time) {
        // swap the processes
        swap(&processes[index], &processes[(index-1)/2]);

        // update the index
        index = (index-1)/2;
    }
}
    
void min_heapify(struct process processes[], int arr_len, int index) {
    int left = 2*index + 1;
    int right = 2*index + 2;
    int smallest = index;
    if (left < arr_len && processes[left].burst_time < processes[smallest].burst_time) {
        smallest = left;
    }
    if (right < arr_len && processes[right].burst_time < processes[smallest].burst_time) {
        smallest = right;
    }
    if (smallest != index) {
        // swap the processes
        swap(&processes[index], &processes[smallest]);
        // min_heapify the rest of the heap
        min_heapify(processes, arr_len, smallest);
    }
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
