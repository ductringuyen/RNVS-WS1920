#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "ntp.h"

#define ntpPort  "123"

int requestNumber;
struct timespec clientClock;

int main(int argc, char** argv) {
    requestNumber = atoi(argv[1]);
    int serverNumber = argc-2;
    double RTT_max;
    double RTT_min;
    double RTT[requestNumber];

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
            status = getaddrinfo(argv[i+2], ntpPort, &hints, &servinfo);
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
            //printf("sendto: %d\n",msglen);
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
            //printf("recvfrom: %d\n", msglen);
            close(socketfd);
            double T4_unix = getTimeStamp(clientClock);

            // Analize the Response
            double T2_unix, T3_unix;
            float rootDispersion;
            analizeTheResponse(ntpResponse,&T2_unix,&T3_unix,&rootDispersion);

            printf("RTT[35]= %lf \n",RTT[35]);
            printf("RTT[34]= %lf \n",RTT[34]);
            printf("RTT[33]= %lf \n",RTT[33]);
            printf("RTT[99]= %lf \n",RTT[99]);
            printf("RTT[36]= %lf \n",RTT[36]);

            RTT[j] = (T4_unix - T1_unix) -(T3_unix - T2_unix);
            //RTT[35] = RTT[34];
            printf("RTT[%d] = %lf \n",j,RTT[j]);
            double delay = RTT[j]/2;
            double offset = (T2_unix - T1_unix + T3_unix -T4_unix)/2;
            double Dispersion_of_8_Anfragen;
            if (j >6) {
                RTT_min = RTT[j-7];
                RTT_max = RTT[j-7];
                for (int a = j - 6; a <= j; a++) {
                    if (RTT[a] < RTT_min) {
                        RTT_min = RTT[a];
                        printf ("min: %lf of RTT: %d \n" ,RTT_min,a);
                    }
                    if (RTT[a] > RTT_max) {
                        RTT_max = RTT[a];
                        printf ("max: %lf of RTT: %d \n" ,RTT_max,a);
                    }
                }
            } else {
                RTT_min = RTT[0];
                RTT_max = RTT[0];
                for (int a = 0; a<=j ;a++) {
                    if (RTT[a] < RTT_min) {
                        RTT_min = RTT[a];
                       // printf ("min: %lf \n" ,RTT_min);
                    }
                    if (RTT[a] > RTT_max) {
                        RTT_max = RTT[a];
                       // printf ("max: %lf \n" ,RTT_max);
                    }
                }
            }
            //if (offset <-100000) offset = 0;
            Dispersion_of_8_Anfragen = RTT_max - RTT_min;
            printf("%s;   %d;    %f;    %lf;    %lf;    %lf\n",argv[i+2],j+1,rootDispersion,Dispersion_of_8_Anfragen,delay,offset);
            //printf("%lf;    %lf;     %lf;      %lf\n",T1_unix,T2_unix,T3_unix,T4_unix);
            sleep(1);
        }
    }
}