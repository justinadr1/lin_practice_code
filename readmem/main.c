#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = (pid_t)strtol(argv[1], NULL, 10);

    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1)
    {
        perror("ptrace(PTRACE_ATTACH)");
        return 1;
    }

    waitpid(pid, NULL, 0);

    char mem_path[64];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);

    int mem_fd = open(mem_path, O_RDONLY);
    if (mem_fd == -1) 
    {
        perror("open(/proc/pid/mem)");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 1;
    }

    while (1) 
    {
        uintptr_t address;
        printf("Enter address (hex): ");
        scanf("%lx", &address);

        unsigned char buffer[16];
        ssize_t bytes_read = pread(mem_fd, buffer, sizeof(buffer), (off_t)address);

        if (bytes_read == -1)
        {
            perror("pread");
        } 
        else 
        {
            printf("16 Bytes read: ");
            for (int i = 0; i < bytes_read; i++) 
            {
                printf("%02X ", buffer[i]);
            }
            printf("\n");
        }
    }

    close(mem_fd);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
}

