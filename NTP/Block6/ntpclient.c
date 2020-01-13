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

#define ntpPort  "123"
#define unix_ntp_time_const 2208988800

#define FRAC       4294967296.             /* 2^32 as a double */
#define LFP2D(a)   ((double)(a) / FRAC)

int requestNumber;
struct timespec clientClock;

int main(int argc, char** argv) {
    requestNumber = atoi(argv[1]);
    int serverNumber = argc-2;
    char *server[serverNumber];
    for (int i = 0; i < serverNumber; i++)
        server[i] = argv[i + 2];

    for (int i = 0; i < serverNumber; i++) {
        for (int j = 0; j < requestNumber; j++) {

            // Get Server Info and send NTP Request
            struct addrinfo hints, *servinfo, *p ;
            unsigned int status;
            int socketfd;		    				   // the send socket

            memset(&hints, 0, sizeof hints);    	   // hints is empty

            hints.ai_family = AF_INET;        	   	   // IPv4
            hints.ai_socktype = SOCK_DGRAM;            // Datagram listener
            hints.ai_flags = AI_PASSIVE;               // Use my IP


            // Get Info of the actual peer
            status = getaddrinfo(server[i], ntpPort, &hints, &servinfo);
            if (status != 0) {
                printf("getaddrinfo error: %s\n",gai_strerror(status));
                exit(1);
            }

            for(p = servinfo; p != NULL; p = p->ai_next) {
                // Create a Socket
                socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
                if (socketfd == -1) {
                    perror("Failed to create a socket\n");
                    continue;
                }

                break;
            }

            // Send the NTP-Request
            clock_gettime(CLOCK_REALTIME, &clientClock);
            double T1_unix = getTimeStamp(clientClock);
            unsigned char* ntpRequest = createNTPRequest(T1_unix);
            int msglen = sendto(socketfd,ntpRequest,48,0,p->ai_addr,p->ai_addrlen);
            if ( msglen == -1) {
                perror("sendto");
                exit(1);
            }
            printf("sendto: %d\n",msglen);
            freeaddrinfo(servinfo);

            // Receive the NTP-Response
            unsigned char* ntpResponse = malloc(48);
            struct sockaddr_storage serverAddrInfo;    	   // connector's addresponses Info
            socklen_t addrSize;
            msglen = recvfrom(socketfd,ntpResponse,48,0,(struct sockaddr *)&serverAddrInfo,&addrSize);
            if (msglen == -1) {
                perror("recvfrom");
                exit(1);
            }
            close(socketfd);

            clock_gettime(CLOCK_REALTIME,&clientClock);
            printf("recvfrom: %d\n", msglen);
            close(socketfd);
            double T4_unix = getTimeStamp(clientClock);

            // Analize the Response
            double T2_unix, T3_unix;
            float rootDispersion;
            analizeTheResponse(ntpResponse,&T2_unix,&T3_unix,&rootDispersion);

            double delay = (T4_unix - T1_unix) -(T3_unix - T2_unix);
            double offset = 0.5*(T2_unix - T1_unix + T3_unix -T4_unix);

            printf("%s;%d;%f;%lf,%lf\n",argv[i+2],j+1,rootDispersion,delay,offset);
        }
    }
}