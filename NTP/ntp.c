#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <time.h>
#include "ntp.h"

#define ntpPort = "123"
#define unix_ntp_time_const 2208988800

void rv_memcpy(void* dst, void* src, unsigned int len) {
  unsigned char* dstByte = (unsigned char*) dst;
  unsigned char* srcByte = (unsigned char*) src;
  for (unsigned int i = 0; i < len; i++) {
    dstByte[i] = srcByte[len-1-i];
  }
}

int bindToPort(char* portString) {
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
    	// Create the Listener Socket
    	listener = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
   		if(listener == -1) {
       	perror("Failed to create a listener\n");
       	continue;
    	}
    	if (bind(listener, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
            close(listener);
            perror("Problems with binding\n");
            continue;
        }
        break; // sucessfully bind
    }
    freeaddrinfo(servinfo);
    //printf("Peer %d: Binding to the listener\n", nodeID);
    return listener;
}

double getTimeStamp(struct timespec* clock) {
	double total;
	total = (double)clock.tv_sec + (double)clock.tv_nsec/(double)1000000000;
	return total;
}

unsigned char* createNTPRequest(double t1_unix) {
	double t1_ntp = t1_unix - unix_ntp_time_const;
	unsigned char* request = calloc(48,1);
	request[0] = 35; 			 		// first Byte: 00|100|011 -> VN = 4, mode = 3
	rv_memcpy(&request[40],&t1_unix,8);	// seal the Transmit Timestamp
	return request;
}