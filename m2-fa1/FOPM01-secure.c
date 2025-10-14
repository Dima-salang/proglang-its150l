// mutual exclusion in C

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define FILE_PATH "FOPM01-secure.c"


char getUserInput(int isLocked);

int main() {
    // file descriptor
    int fd;
    char input;

    // flock struct
    struct flock lock;

    // prompt the user to lock the file
    do {
        input = getUserInput(0);
    } while (input != '\n');

    // open the file
    fd = open(FILE_PATH, O_RDWR);

    if (fd == -1) {
        fprintf(stderr, "Failed to open file! Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }


    // lock the file
    printf("Attempting to acquire lock on file...\n");
    // get the lock to see if the file is locked
    if (fcntl(fd, F_GETLK, &lock) == -1) {
        fprintf(stderr, "Failed to get lock! Error: %s\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (lock.l_type != F_UNLCK) {
        fprintf(stderr, "File is already locked by process %d\n", lock.l_pid);
    }

    // lock
    // set up the lock
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        fprintf(stderr, "Failed to lock file! Error: %s\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Acquired lock! ");
    printf("File locked by process %d\n", getpid());


    // prompt the user to unlock the file
    do {
        char input = getUserInput(1);
    } while (input != '\n');

    // unlock the file
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        fprintf(stderr, "Failed to unlock file! Error: %s\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("File unlocked!\n");

    // close the file
    close(fd);

    return 0;
}


// function for getting <RETURN> input from user for locking and unlocking
char getUserInput(int isLocked) {
    if (isLocked) {
        printf("Press Enter to unlock the lock to %s\n", FILE_PATH);
    } else {
        printf("Press Enter to acquire the lock to %s.\n", FILE_PATH);
    }
    // read input from user
    char input = getchar();
    return input;
}