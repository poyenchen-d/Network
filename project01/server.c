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

#define SA struct sockaddr

typedef struct IPHeader{
    uint8_t version_ihl;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t source_ip;
    uint32_t destination_ip;
    uint32_t options;
    
}IPHeader;

typedef struct UDPHeader
{
  uint32_t source_port:16,
           dest_port:16;
  uint32_t Segment_Length:16,
           Checksum:16;
}UDPHeader;

typedef struct MACHeader{
    uint8_t sour_mac[6];        // source
    uint8_t des_mac[6];         // destination
    uint16_t fram_typ;        // frame type
    uint32_t crc;             // crc
}MACHeader; //18bytes

typedef struct Packet
{
  struct IPHeader ipheader;
  struct UDPHeader udpheader;
  struct MACHeader macheader;
  char buffer[MTU - 40];
}Packet;

int count = 0;

void udp_msg_sender(int fd, struct sockaddr* dst)
{
    int payload_size;
    
    //create MAC header
    	struct MACHeader *machdr = (struct MACHeader *)malloc(sizeof(struct MACHeader));
    	unsigned char sour[] = {0x12,0x34,0x56,0x78,0x90,0x98};
    	unsigned char des[] = {0x21,0x43,0x65,0x87,0x90,0x89}; 
    	memcpy(machdr->sour_mac, sour , sizeof(sour));
    	memcpy(machdr->des_mac, des , sizeof(des));  	
    	machdr->fram_typ = 0x0000; 
    	machdr->crc = 0x00000000;
     
    //create IP header
	struct IPHeader *iphdr = (struct IPHeader *)malloc(sizeof(struct IPHeader));
	iphdr->version_ihl = 0x45;
	iphdr->total_length = MTU;
	iphdr->identification = 0xAAAA;
	iphdr->flags_fragment_offset = 0x4000;
	iphdr->time_to_live = 100;
	iphdr->protocol = 0x11; //udp
	iphdr->header_checksum = 0;
	iphdr->source_ip = 0x0A115945;
	iphdr->destination_ip = 0x0A000301;
	iphdr->options = 0;
	
    //create UDP header
	struct UDPHeader *udphdr = (struct UDPHeader *)malloc(sizeof(struct UDPHeader));
	udphdr->source_port = 10000;
	udphdr->dest_port = 10010;
	udphdr->Segment_Length = MTU - 38;
	udphdr->Checksum = 0; 
    
    	

    payload_size = MTU - sizeof(*iphdr) - sizeof(*udphdr) - sizeof(*machdr);
    

    
    char buf[payload_size];
    for(int i=0 ; i<payload_size ; i++)
    {
    	buf[i] = 1;
    }
    
    struct Packet *packet = (struct Packet *)malloc(sizeof(struct Packet));

    packet->ipheader = *iphdr;
    packet->udpheader = *udphdr;
    packet->macheader = *machdr;


        
    socklen_t len;
    
    int cnt=0;

    while(cnt<10)
    {     
    	cnt+=1;   
        packet->ipheader = *iphdr;
	packet->udpheader = *udphdr;
	packet->macheader = *machdr;
        len = sizeof(*dst);
        sendto(fd, packet, sizeof(*packet), 0, dst, len);
	printf("server send UDP packet %d !\n", cnt );
	sleep(1);
    }
}

int tcpsocket(){
	
    int count=0;
    while(count<10){ 
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[PACKET_SIZE] = {0};

    // Socket creation
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    // port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // listen 
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Waiting for connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    
    
    read(new_socket, buffer, PACKET_SIZE);
    cout<<"-----server receive-----"<<endl; 
    cout<<"payload : ";
      
    for(int i=60;i<64;i++){
    	cout<<buffer[i];
    }
    cout<<endl<<count;
    cout<<endl<<endl;
    count++;
    close(new_socket);
    close(server_fd);
    }

  
    return 0;
}

int udpsocket()
{
    int server_fd;
    struct sockaddr_in ser_addr;

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }


    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
    ser_addr.sin_port = htons(ROUTER_PORT);

    udp_msg_sender(server_fd, (struct sockaddr*)&ser_addr);
    
    close(server_fd);

    return 0;
}

int main()
{
        tcpsocket();
        sleep(1);
	udpsocket();
	
	return 0;
}
