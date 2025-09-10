// required libraries
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h> 

// prototypes
void printRandomArray(int *randomNums);
int sumArray(int *randomNums);

int main() {
    pid_t pid;

    // pid of the parent process
    pid_t its150l_fopm01_pid = getpid();

    // exit value
    int gotNum;

    int randomNum;
    int randomNums[5];

    for (int i = 0; i < 5; i++) {
        pid = fork();
        if (pid == -1) {
            printf("Fork failed\n");
        } else if (pid == 0) {
            // seed the rng with the pid of the child process
            srand(getpid());

            // calculate random number from 1-10
            randomNum = (rand() % 10) + 1;
            printf("[Child Process] Child process %d has been created from parent process %d with random number %d\n", getpid(), getppid(), randomNum);
            sleep(1);
            exit(randomNum);
        } else {
            printf("[Parent Process] Process %d has created child process %d\n", its150l_fopm01_pid, pid);
            // wait for the child process to finish and store the exit value in gotNum
            wait(&gotNum);
            randomNums[i] = WEXITSTATUS(gotNum);
        }
    }
    // print the numbers in the randomNums array
    printf("\n");
    printRandomArray(randomNums);

    // sum all of the numbers in the randomNums array
    int sum = sumArray(randomNums);
    printf("Sum of random numbers: %d\n", sum);

    return 0;
}


void printRandomArray(int *randomNums) {
    printf("Random Nums Array: [");
    for (int i = 0; i < 5; i++) {
        printf(" %d ", randomNums[i]);
    }
    printf("]\n");
}

int sumArray(int *randomNums) {
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += randomNums[i];
    }
    return sum;
}