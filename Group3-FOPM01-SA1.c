#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FIFO_NAME "/tmp/my_fifo"

int main() {
    // messages
    const char PARENT_MSG[] = "We will strive for the fame and the glory";
    const char CHILD_MSG[] = "Of Mapua University!";

    pid_t pid;
    int fd;

    // create the FIFO file
    mkfifo(FIFO_NAME, 0666);

    // fork the process
    pid = fork();

    char buffer[100];

    // fork failed
    if (pid == -1) {
        printf("Fork failed\n");
        return 1;
    }

    // child process
    if (pid == 0) {
        printf("Child process\n");

        // open the FIFO file for reading
        fd = open(FIFO_NAME, O_RDONLY);
        read(fd, buffer, sizeof(buffer)-1);
        printf("Child process received: %s\n", buffer);

        sleep(1);


        // write to the FIFO file
        fd = open(FIFO_NAME, O_WRONLY);

        // process gets blocked here because the parent process has not yet opened the FIFO file for reading
        // until the parent process opens the FIFO file for reading, the child process will not proceed

        write(fd, CHILD_MSG, strlen(CHILD_MSG));
        printf("Child process sent: %s\n", CHILD_MSG);

        sleep(1);


        close(fd);
    } 
    // parent process
    else {
        printf("Parent process\n");

        // open the FIFO file for writing
        fd = open(FIFO_NAME, O_WRONLY);

        // process gets blocked here because the child process has not yet opened the FIFO file for reading
        // until the child process opens the FIFO file for reading, the parent process will not proceed

        write(fd, PARENT_MSG, strlen(PARENT_MSG));
        printf("Parent process sent: %s\n", PARENT_MSG);

        sleep(1);


        // open the FIFO file for reading
        fd = open(FIFO_NAME, O_RDONLY);
        read(fd, buffer, sizeof(buffer)-1);
        printf("Parent process received: %s\n", buffer);


        close(fd);

        // remove the FIFO file
        unlink(FIFO_NAME);
        wait(NULL);
    }

    return 0;
}
