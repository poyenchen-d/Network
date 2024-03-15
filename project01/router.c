#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define CLIENT_IP "127.0.0.1"
#define SERVER_IP "127.0.0.2"
#define ROUTER_PORT 9002
#define CLIENT_PORT 9003
#define SERVER_PORT 8082

#define SA struct sockaddr

int main()
{
    int router_fd, client_fd, server_fd;
    struct sockaddr_in router_addr, client_addr, server_addr;

    // Create router socket
    router_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (router_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind router socket to router port
    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = INADDR_ANY;
    router_addr.sin_port = htons(ROUTER_PORT);
    if (bind(router_fd, (SA *)&router_addr, sizeof(router_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len;
    char buffer[1024];

    // Router loop
    while (1)
    {
        len = sizeof(client_addr);
        recvfrom(router_fd, buffer, sizeof(buffer), 0, (SA *)&client_addr, &len);

        if (strcmp(buffer, "EXIT") == 0)
        {
            printf("Router shutting down\n");
            break;
        }

        printf("Received packet from client\n");

        if (client_fd == 0)
        {
            // Create client socket if not already created
            client_fd = socket(AF_INET, SOCK_DGRAM, 0);
        }

        // Send packet to server
        sendto(client_fd, buffer, sizeof(buffer), 0, (SA *)&server_addr, sizeof(server_addr));

        len = sizeof(server_addr);
        recvfrom(client_fd, buffer, sizeof(buffer), 0, (SA *)&server_addr, &len);

        printf("Received packet from server\n");

        // Send packet back to client
        sendto(router_fd, buffer, sizeof(buffer), 0, (SA *)&client_addr, sizeof(client_addr));
    }

    close(router_fd);
    close(client_fd);
    close(server_fd);

    return 0;
}
