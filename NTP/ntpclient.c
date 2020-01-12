#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>   
#include "ntp.h"

#define ntpPort = "123"
#define unix_ntp_time_const 2208988800

int requestNumber;
char** server;
struct timespec clock;

int main(int argc, char** argv) {
	requestNumber = atoi(argv[1]);
	int serverNumber = argc-2; 
	for (int i = 0; i < serverNumber; i++) {
		server[i] = argv[i+2];
	}

	for (int i = 0; i < serverNumber; i++) {
		for (int i = 0; i < requestNumber; i++) {
			int receiveSocket = bindToPort(ntpPort);
			
			// Get Server Info and send NTP Request
			struct addrinfo hints, *servinfo;
    		unsigned int status;
    
    		memset(&hints, 0, sizeof hints);    	   // hints is empty 
    		struct sockaddr_storage addrInfo;    	   // connector's addresponses Info
    		socklen_t addrSize;

    		hints.ai_family = AF_INET;        	   	   // IPv4
    		hints.ai_socktype = SOCK_DGRAM;            // Datagram listener
    		hints.ai_flags = AI_PASSIVE;               // Use my IP


    		// Get Info of the actual peer   
    		status = getaddrinfo(NULL, portString, &hints, &servinfo);
    		if (status != 0) {
    		    printf("getaddrinfo error: %s\n",gai_strerror(status));
    		    exit(1);
    		}

    		while(1) {
    			// Create a Socket
    			socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
   				if(socket == -1) {
    		   		perror("Failed to create a socket\n");
    		   		continue;
    			}
    			break;
    		}
    		
    		// Send the NTP-Request
    		clock_gettime(CLOCK_REALTIME, &clock);
    		double T1_unix = getTimeStamp(&clock);
    		unsigned char* ntpRequest = createNTPRequest(T1_unix);

    		freeaddrinfo(servinfo); 
		
		}
	}
}