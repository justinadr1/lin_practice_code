#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>        // close()
#include <arpa/inet.h>     // inet_addr(), htons()
#include <sys/socket.h>    // socket(), connect()
#include <netinet/in.h>    // struct sockaddr_in
#include <errno.h>

int main() 
{
    char ip[64];
    int port;

    printf("Enter IP address (e.g., 127.0.0.1): ");
    scanf("%63s", ip);

    printf("Enter port: ");
    scanf("%d", &port);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) 
    {
        perror("Failed to create socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    printf("Checking %s:%d ...\n", ip, port);

    int result = connect(s, (struct sockaddr*)&addr, sizeof(addr));
    if (result == 0)
        printf("Port %d is OPEN.\n", port);
    else
        printf("Port %d is CLOSED. Error: %s\n", port, strerror(errno));

    close(s);
    return 0;
}

