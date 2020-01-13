
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

#define FRAC       4294967296.             /* 2^32 as a double */
#define LFP2D(a)   ((double)(a) / FRAC)

void rv_memcpy(void* dst, void* src, unsigned int len) {
    unsigned char* dstByte = (unsigned char*) dst;
    unsigned char* srcByte = (unsigned char*) src;
    for (unsigned int i = 0; i < len; i++) {
        dstByte[i] = srcByte[len-1-i];
    }
}

double getTimeStamp(struct timespec clientClock) {
    double total;
    total = (double)clientClock.tv_sec + (double)clientClock.tv_nsec/(double)1000000000;
    return total;
}

unsigned char* createNTPRequest(double t1_unix) {
    double t1_ntp = t1_unix + unix_ntp_time_const;
    unsigned char* request = calloc(48,1);
    request[0] = 35; 			 		// first Byte: 00|100|011 -> VN = 4, mode = 3
    rv_memcpy(&request+40,&t1_ntp,8);	// seal the Transmit Timestamp
    return request;
}

void analizeTheResponse(unsigned char* response, double* T2, double* T3, float* rootDispersion) {
    unsigned int *T2a = malloc(4);
    unsigned int *T2b = malloc(4);
    unsigned int *T3a = malloc(4);
    unsigned int *T3b = malloc(4);
    rv_memcpy(rootDispersion,response+8,4);
    rv_memcpy(T2a,response+32,4);
    rv_memcpy(T2b,response+36,4);
    rv_memcpy(T3a,response+40,4);
    rv_memcpy(T3b,response+44,4);
    printf("T2a: %u\n", *T2a);
    printf("T2b: %u\n", *T2b);
    printf("T3a: %u\n", *T3a);
    printf("T3b: %u\n", *T3b);
    *T2 = *T2a + LFP2D(*T2b) - unix_ntp_time_const;
    *T3 = *T3a + LFP2D(*T3b) - unix_ntp_time_const;
    free(T2a);
    free(T2b);
    free(T3a);
    free(T3b);
}