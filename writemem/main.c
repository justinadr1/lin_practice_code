#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = (pid_t)strtol(argv[1], NULL, 10);

    /* Attach to target process */
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) 
    {
        perror("ptrace(PTRACE_ATTACH)");
        return 1;
    }

    /* Wait until the process is stopped */
    waitpid(pid, NULL, 0);

    char mem_path[64];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);

    int mem_fd = open(mem_path, O_RDWR);
    if (mem_fd == -1)
    {
        perror("open(/proc/pid/mem)");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 1;
    }

    while (1)
    {
        uintptr_t addr;
        uint64_t value;

        printf("Enter address (hex): ");
        scanf("%lx", &addr);

        printf("Enter 8-byte value (hex): 0x");
        scanf("%lx", &value);

        ssize_t written = pwrite(mem_fd, &value, sizeof(value), (off_t)addr);

        if (written == -1) 
        {
            perror("pwrite");
        } 
        else 
        {
            printf("Wrote %zd bytes\n", written);
        }
    }

    close(mem_fd);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    return 0;
}

