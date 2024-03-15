#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

#define MTU 1500
#define BUFF_LEN 10000  //buffer size
#define PACKET_SIZE 1518
#define CLIENT_IP "127.0.0.1"
#define SERVER_IP "127.0.0.2"
#define SERVER_PORT 9000
#define ROUTER_PORT 9002
#define CLIENT_PORT 9003

int main() {
    // from client
    int router_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[PACKET_SIZE] = {0};

    // Socket creation
    if ((router_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(ROUTER_PORT);

    // port
    if (bind(router_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // listen 
    if (listen(router_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Waiting for connection
    if ((new_socket = accept(router_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Clear buffer
        memset(buffer, 0, PACKET_SIZE);

        // Read from client
        read(new_socket, buffer, PACKET_SIZE);
        printf("-----server receive-----\n"); 
         for(int i=60;i<64;i++){
    	cout<<buffer[i];
    }
    cout<<endl<<endl;
    
        printf("-----buffer-----\n"); 

        // to server
        // Create socket
        int server_fd;
        struct sockaddr_in server_addr;
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }

        // Configure server address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        // Connect to server
        if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection to server failed");
            exit(EXIT_FAILURE);
        }

        printf("Connected to server\n");

        // Send to server
        send(server_fd, buffer, PACKET_SIZE , 0);

        // Close socket
        close(server_fd);
    }

    // Close socket
    close(new_socket);
    close(router_fd);

    return 0;
}

