#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "uthash.h"
#include "hashing.h"
#include "dht.h"

#define ACK 8
#define GET 4
#define SET 2
#define DEL 1
#define BACKLOG 10

#define LOOKUP 129
#define REPLY 130
#define STABILIZE 132
#define NOTIFY 136
#define JOIN 144
#define FACK 160
#define FINGER 192 

#define FINAL 13
#define HASH 5

#define unknownPeer 0
#define thisPeer 1
#define nextPeer 2

#define unknown 101


// For the Peers
unsigned int nodeID;
unsigned int prevID;
unsigned int nextID;

unsigned int nodeIP;
unsigned int nextIP;
unsigned int prevIP;

unsigned int nodePort;
unsigned int prevPort;
unsigned int nextPort;

unsigned int friendID;
unsigned int friendIP;
unsigned int friendPort;

unsigned int clientSocket;  // Socket to the sender of the Hash Request
pthread_mutex_t mutex; 

// STABILIZE thread
void* stabilizing(void* arg) {
	while(1) {
		pthread_mutex_lock(&mutex);
		while (nextID < unknown) {
			pthread_mutex_unlock(&mutex);
			unsigned char* hashID = calloc(2,1);
			unsigned char* stabilizeRequest = createPeerRequest(hashID,nodeID,nodeIP,nodePort,STABILIZE);
            //create Connection to the next Peer
            pthread_mutex_lock(&mutex);
            char ipString[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &nextIP, ipString, sizeof(ipString));
            char portString[20];
            itoa(nextPort,portString);
            unsigned int socket = createConnection(ipString,portString,NULL);                            
            //send STABILIZE Request
            printf("Peer %d Stabilizing to Peer %d\n", nodeID, nextID);
            pthread_mutex_unlock(&mutex);
            if (send(socket,stabilizeRequest,11,0) == -1) {
                perror("Error in sending\n");
            }
            sleep(2);
            pthread_mutex_lock(&mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char** argv) {

	prevID = unknown;
	nextID = unknown;

    if (argc == 3) {
    	nodeID = 0;
    	nodePort = atoi(argv[2]);
    } else if (argc == 4) {
    	nodeID = atoi(argv[3]);
    	nodePort = atoi(argv[2]);
    } else if (argc == 6) {
    	nodeID = atoi(argv[3]);
    	nodePort = atoi(argv[2]);
    	friendPort = atoi(argv[5]);
    }

    // For the Select function 
    unsigned int requestType;           // What kind of Connector, Clients or Peers?
    fd_set master;    					// master file descriptor list
    fd_set read_fds;  					// temp file descriptor list for select()
    unsigned int fdmax;        			// maximum file descriptor number 		

    
    /*-------------------------------------------- GET PEER INFO --------------------------------------------------*/
    struct addrinfo hints, *servinfo;
    unsigned int status;
    
    memset(&hints, 0, sizeof hints);    	   // hints is empty 
    unsigned int listener, nextSocket, newSocketFD, clientSocket, firstPeerSocket, chosenPeerSocket;
    struct sockaddr_storage addrInfo;    	   // connector's addresponses Info
    socklen_t addrSize;

    hints.ai_family = AF_INET;        	   	   // IPv4
    hints.ai_socktype = SOCK_STREAM;           // Stream listener
    hints.ai_flags = AI_PASSIVE;               // Use my IP


    // Get Info of the actual peer   
    status = getaddrinfo(NULL, argv[2], &hints, &servinfo);
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
    
    // Get IP of Peer
    struct sockaddr_in *ipv4 = (struct sockaddr_in*) servinfo->ai_addr;
    nodeIP = *(unsigned int*)(&ipv4->sin_addr); //////////// Where magic happen /////////////////
    freeaddrinfo(servinfo);
    printf("Peer %d: Binding to the listener\n", nodeID);

    /*---------------------------------------------------------------------------------------------------------------*/

    // Create Hash Table
    hashable *hTab = NULL; // Hash Table must be set to NULL at first 
    hash_request_info_list* hashRequestInfoList = malloc(sizeof(hash_request_info_list*));
    hashRequestInfoList->head = NULL;

    // LISTEN WITH THE LISTENER
    if (listen(listener, BACKLOG) == -1) {
        perror("Can't hear anything");
        exit(1);
    }

    FD_ZERO(&master);          // clear the master and temp set
    FD_ZERO(&read_fds);	
    FD_SET(listener, &master); // add the listener to the master set
    fdmax = listener;		   // keep track of the biggest file descriptor

    /*-------------------------------------------------- JOINING ---------------------------------------------------*/
    if (argc == 6) {
    	printf("Peer %d: I want to join\n", nodeID);
    	unsigned int friendSocket = createConnection(argv[4],argv[5],&friendIP);
    	//printf("Peer %d: got connected\n", nodeID);
    	unsigned char* hashID = calloc(2,1);
    	unsigned char* joinRequest = createPeerRequest(hashID,nodeID,nodeIP,nodePort,JOIN);
    	//printf("Peer %d: Join Request created\n", nodeID);
    	if (send(friendSocket,joinRequest,11,0) == -1) {
            perror("Error in sending\n");
        }
        printf("Peer %d: Join Request sent\n", nodeID);
    }
    /*--------------------------------------------------------------------------------------------------------------*/
    
    if (pthread_mutex_init(&mutex,NULL) != 0) {
    	printf("Mutex init failed\n");
    	exit(1);
    }
    pthread_t stabilizingThread;
    if (pthread_create(&stabilizingThread,NULL,stabilizing,NULL) != 0) {
    	printf("Thread init failed\n");
    	exit(1);
    }

    while(1){

        read_fds = master;    // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(unsigned int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {    
                    // handle new connections
                    addrSize = sizeof(addrInfo);
                    newSocketFD = accept(listener,(struct sockaddr *)&addrInfo,&addrSize);
                    if (newSocketFD == -1) {
                        perror("Unacceptable");
                    } else {
                        FD_SET(newSocketFD, &master); // add to master set
                        if (newSocketFD > fdmax){     // keep track of the max
                            fdmax = newSocketFD;
                        }
                    }
                } else {
                    unsigned char* firstByte = malloc(1);
                    // handle data from a connector
                    // Get the first Byte
                    unsigned int msglen = recv(i, firstByte, 1, 0);
                    if (msglen <= 0) {
                        perror("Error in receiving");
                        close(i);           
                        FD_CLR(i, &master); 
                    }
                    requestType = firstByteDecode(firstByte);
                    //printf("Peer %d got Something: decode the first Byte %d\n", nodeID, requestType);                                               
                                        
                    if (requestType == HASH) {
                        printf("Peer %d: received a Hash Request\n", nodeID);
                        hash_request_info* hashRequestInfo;
                        hashRequestInfo = getHashRequestInfo(i,firstByte);
                        put_in_the_list(hashRequestInfoList, hashRequestInfo);
                                                                      
                        unsigned char* hashKey;
                        if (hashRequestInfo->keyLen == 1) {
                            hashKey = malloc(2);
                            hashKey[0] = hashRequestInfo->key[0];
                            hashKey[1] = 0;
                        } else {
                            hashKey = hashRequestInfo->key;
                        }

                        unsigned int hashValue = ringHashing(hashKey);
                        printf("Peer %d: hashValue is %d\n", nodeID, hashValue);
                        if (checkPeer(nodeID,prevID,nextID,hashValue) == thisPeer) {        // This peer is responsible for this Request
                            printf("Peer %d: I'm responsible for the request\n", nodeID);
                            unsigned char* response;
                            unsigned int responseLen; 
                            response = peerHashing(&hTab,hashRequestInfo,&responseLen);
                            if (send(i,response,responseLen,0) == -1) {
                                perror("Error in sending\n");
                            }
                            remove_info_from_list(hashRequestInfoList, hashRequestInfo);
                            freeInfo(hashRequestInfo);
                            printf("peer %d: Hash request sent\n", nodeID);
                            close(i);
                            FD_CLR(i, &master); 
                        }
                        else if (checkPeer(nodeID,prevID,nextID,hashValue) == nextPeer) {   // Next peer is responsible for this Request
                            printf("Peer %d: my next pal %d is responsible for the request\n", nodeID,nextID);
                            //create Connection to the next Peer
                        	char ipString[INET_ADDRSTRLEN];
                        	inet_ntop(AF_INET, &nextIP, ipString, sizeof(ipString));
                        	char portString[20];
                        	itoa(nextPort,portString);
                        	nextSocket = createConnection(ipString,portString,NULL);
                        	//create Hash Request
                        	unsigned char* hashRequest = createHashRequest(hashRequestInfo);
                        	//send Hash Request
                        	unsigned int msglen = 7 + hashRequestInfo->keyLen + hashRequestInfo->valueLen;
                            if (send(nextSocket,hashRequest,msglen,0) == -1) {
                                perror("Error in sending\n");
                            }

                            hashRequestInfo->finalSocket = nextSocket;
                            FD_SET(nextSocket, &master); // add to master set
                            if (nextSocket > fdmax){     // keep track of the max
                                fdmax = nextSocket;
                            }
                        } 
                        else {                                                               // unknown Peer
                            printf("Peer %d: I dunno but I'll ask my next pal %d\n", nodeID,nextID);                                          
                            unsigned char* peerRequest;
                            //create LOOKUP Request
                            printf("Peer %d: my IP is %d\n", nodeID, nodeIP);
                            peerRequest = createPeerRequest(hashKey,nodeID,nodeIP,nodePort,LOOKUP);
                            //create Connection to the next Peer
                        	char ipString[INET_ADDRSTRLEN];
                        	inet_ntop(AF_INET, &nextIP, ipString, sizeof(ipString));
                        	char portString[20];
                        	itoa(nextPort,portString);
                        	nextSocket = createConnection(ipString,portString,NULL);                            
                            //send LOOKUP Request
                            if (send(nextSocket,peerRequest,11,0) == -1) {
                                perror("Error in sending\n");
                            }
                        }

                    } else if (requestType == LOOKUP) {
                        //get full request
                        printf("Peer %d: received a LOOKUP Request\n", nodeID);
                        unsigned char* peerRequest;
                        peerRequest = getPeerRequest(i,firstByte);
                        unsigned char* hashID = malloc(2);
                        memcpy(hashID,peerRequest+1,2);
                        unsigned int hashValue = ringHashing(hashID);
                        printf("Peer %d: hashValue is %d\n", nodeID, hashValue);
                        
                        // There won't be the case of thisPeer with LOOKUP
                        
                        if (checkPeer(nodeID,prevID,nextID,hashValue) == nextPeer) {
                            printf("Peer %d: my next pal %d is responsible for the request\n", nodeID,nextID);
                            unsigned char* hashID = malloc(2);
                            memcpy(hashID,peerRequest+1,2);

                            // Get Info of the original Peer
                            unsigned int firstPeerIP;
                            memcpy(&firstPeerIP,peerRequest+5,4);
                            char ipString[INET_ADDRSTRLEN];
                            inet_ntop(AF_INET, &firstPeerIP, ipString, sizeof(ipString));

                            unsigned int firstPeerPort;
                            rv_memcpy(&firstPeerPort,peerRequest+9,2);
                            char portString[20];
                            itoa(firstPeerPort,portString);
                            
                            // Get the next IP
                            //status = getaddrinfo(argv[8], argv[9], &hints, &servinfo);
                            //if (status != 0) {
                            //    printf("getaddrinfo error: %s\n",gai_strerror(status));
                            //    exit(1);
                            //}    
                            //struct sockaddr_in *ipv4 = (struct sockaddr_in*) servinfo->ai_addr;
                            //nextIP = *(unsigned int*)(&ipv4->sin_addr); //////////// Where magic happen /////////////////
                            //freeaddrinfo(servinfo);
                            // Got the next IP

                            peerRequest = createPeerRequest(hashID,nextID,nextIP,nextPort,REPLY);
                            // Connect to the first peer
                            firstPeerSocket = createConnection(ipString,portString,NULL);

                            if (send(firstPeerSocket,peerRequest,11,0) == -1) {
                                perror("Error in sending\n");
                            }  
                        } else if (checkPeer(nodeID,prevID,nextID,hashValue) == unknownPeer) {
                            printf("Peer %d: I dunno but I'll ask my next pal %d\n", nodeID,nextID);
                            //create Connection to the next Peer
                        	char ipString[INET_ADDRSTRLEN];
                        	inet_ntop(AF_INET, &nextIP, ipString, sizeof(ipString));
                        	char portString[20];
                        	itoa(nextPort,portString);
                        	nextSocket = createConnection(ipString,portString,NULL);
                            if (send(nextSocket,peerRequest,11,0) == -1) {
                                perror("Error in sending\n");
                            }
                        } 
                        close(i);
                        FD_CLR(i, &master);

                    } else if (requestType == REPLY) {
                        //get full request
                        printf("Peer %d: received a REPLY Request\n", nodeID);
                        unsigned char* peerRequest;
                        peerRequest = getPeerRequest(i,firstByte);

                        unsigned int chosenPeerIP;
                        memcpy(&chosenPeerIP,peerRequest+5,4);
                        char ipString[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &chosenPeerIP, ipString, sizeof(ipString));
                        printf("Peer %d: IP of the chosen One: %s\n",nodeID,ipString);

                        unsigned int chosenPeerPort;
                        rv_memcpy(&chosenPeerPort,peerRequest+9,2);
                        char portString[20];
                        itoa(chosenPeerPort,portString);
                        printf("Peer %d: Port of the chosen One: %s\n",nodeID,portString);

                        unsigned char* hashID = malloc(2);
                        memcpy(hashID,peerRequest+1,2);
                        
                        //connect and send Hash Request to the chosen one
                        chosenPeerSocket = createConnection(ipString,portString,NULL);
                        hash_request_info* hashRequestInfo = findHashRequestInfo(hashRequestInfoList,hashID);
                        hashRequestInfo->finalSocket = chosenPeerSocket;
                        free(hashID);
                        unsigned char* hashRequest = createHashRequest(hashRequestInfo);
                        unsigned int msglen = 7 + hashRequestInfo->keyLen + hashRequestInfo->valueLen;
                        if (send(chosenPeerSocket,hashRequest,msglen,0) == -1) {
                            perror("Error in sending\n");
                        }

                        FD_SET(chosenPeerSocket, &master); // add to master set
                        if (chosenPeerSocket > fdmax){     // keep track of the max
                            fdmax = chosenPeerSocket;
                        }
                        close(i);
                        FD_CLR(i, &master);

                    } else if (requestType == STABILIZE) {
                        //get full request
                        unsigned char* peerRequest;
                        peerRequest = getPeerRequest(i,firstByte);

                        unsigned int callerID = 0;
                        unsigned int callerIP = 0;
                        unsigned int callerPort = 0;

                        rv_memcpy(&callerID,peerRequest+3,2);
                        printf("Peer %d: received a STABILIZE Request from Peer %d\n", nodeID, callerID);

                        memcpy(&callerIP,peerRequest+5,4);
                        char ipString[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &callerIP, ipString, sizeof(ipString));
                        //printf("Peer %d: IP of the Predecessor: %s\n",nodeID,ipString);

                        rv_memcpy(&callerPort,peerRequest+9,2);
                        char portString[20];
                        itoa(callerPort,portString);
                        //printf("Peer %d: Port of the Predecessor: %s\n",nodeID,portString);

                        if (prevID == unknown) {
                        	prevID = callerID;
                        	prevIP = callerIP;
                        	prevPort = callerPort;
                        	printf("Peer %d: my prev is %d\n", nodeID, prevID);
                        }

                        unsigned char* hashID = malloc(2);
                        memcpy(hashID,peerRequest+1,2);

                        unsigned char* notifyResponse = createPeerRequest(hashID,prevID,prevIP,prevPort,NOTIFY);
                        unsigned int callerSocket = createConnection(ipString,portString,NULL);
                        if (send(callerSocket,notifyResponse,11,0) == -1) {
                            perror("Error in sending\n");
                        }
                        close(i);
                        FD_CLR(i, &master);

                    } else if (requestType == NOTIFY) {
                    	//get full request
                        unsigned char* peerRequest;
                        peerRequest = getPeerRequest(i,firstByte);

                        unsigned int notifiedID  = 0;
                        rv_memcpy(&notifiedID,peerRequest+3,2);
                        printf("Peer %d: Notified ID is %d\n", nodeID, notifiedID);

                        if (notifiedID == nodeID) {
                           	continue;
                        }
                        
                        pthread_mutex_lock(&mutex); 
                        if (notifiedID > nodeID || (notifiedID < nodeID && nextID == unknown)) {
                        	nextID = notifiedID;
                        	memcpy(&nextIP,peerRequest+5,4);
                        	rv_memcpy(&nextPort,peerRequest+9,2);
                        }
                        pthread_mutex_unlock(&mutex);

                        close(i);
                        FD_CLR(i, &master);
                    
                    } else if (requestType == JOIN) {
                    	printf("Peer %d: received a Join Request\n", nodeID);
                        unsigned char* peerRequest;
                        peerRequest = getPeerRequest(i,firstByte);

                        unsigned int callerID = 0;
                        unsigned int callerIP = 0;
                        unsigned int callerPort = 0;

                        rv_memcpy(&callerID,peerRequest+3,2);
                        memcpy(&callerIP,peerRequest+5,4);
                        rv_memcpy(&callerPort,peerRequest+9,2);
                        
                        pthread_mutex_lock(&mutex);
                        if (checkJoinPeer(nodeID, prevID, nextID, callerID) == thisPeer) {
                        	// Connect to the Joining Caller
                        	char ipString[INET_ADDRSTRLEN];
                        	inet_ntop(AF_INET, &callerIP, ipString, sizeof(ipString));
                        	char portString[20];
                        	itoa(callerPort,portString);
                        	printf("Peer %d: I'm responsible for the Joining of Peer %d\n", nodeID, callerID);
                        	//printf("Peer %d: Join Caller ID is %d\n", nodeID, callerID);
                        	//printf("Peer %d: Join Caller IP is %s\n", nodeID, ipString);
                        	//printf("Peer %d: Join Caller Port is %d\n", nodeID, callerPort);
                        	unsigned int callerSocket = createConnection(ipString,portString,NULL);
                        	printf("Peer %d: I'm about to send a Notify, Caller IP is: %s\n", nodeID, ipString);
                        	// Create and send the Notify Response
                        	unsigned char* hashID = calloc(2,1);
                        	unsigned char* notifyResponse = createPeerRequest(hashID,nodeID,nodeIP,nodePort,NOTIFY);
                        	if (send(callerSocket,notifyResponse,11,0) == -1) {
                            	perror("Error in sending\n");
                        	}
                        	prevID = callerID;
                        	prevIP = callerIP;
                        	prevPort = callerPort;

                        	if (nextID == unknown) {
                        		nextID = callerID;
                        		nextIP = callerIP;
                        		nextPort = callerPort;
                        	}
                        	printf("Peer %d: my next and my prev are %d and %d\n", nodeID, nextID,prevID);
                        	
                        } else if (checkJoinPeer(nodeID, prevID, nextID, callerID) == unknownPeer) {
                        	// Connect to the next Peer
                        	printf("Peer %d on Joining: I dunno but I'll ask my next Peer %d\n", nodeID, nextID);
                        	char ipString[INET_ADDRSTRLEN];
                        	inet_ntop(AF_INET, &nextIP, ipString, sizeof(ipString));
                        	char portString[20];
                        	itoa(nextPort,portString);
                        	nextSocket = createConnection(ipString,portString,NULL);
                        	// Forward the Join Request
                        	if (send(nextSocket,peerRequest,11,0) == -1) {
                            	perror("Error in sending\n");
                        	}
                        }
                        pthread_mutex_unlock(&mutex);
                        close(i);
                        FD_CLR(i, &master);
                    
                    } else if (requestType == FACK) {
                       
                    } else if (requestType == FINGER) {

                    } else if (requestType == FINAL) {
                    	hash_request_info* hashRequestInfo;
                        printf("Peer %d: received a FINAL Request\n", nodeID);
                        hashRequestInfo = getHashRequestInfo(i,firstByte);
                        unsigned char* finalResponse = createHashRequest(hashRequestInfo);
                        hash_request_info* clientRequestInfo;
                        clientRequestInfo = getClientRequestInfo(hashRequestInfoList, hashRequestInfo, i);
                        clientSocket = clientRequestInfo->callerSocket;
                        printf("Peer %d: Got the final Response\n",nodeID);
                        unsigned int msglen = hashRequestInfo->keyLen + hashRequestInfo->valueLen +7;
                        if (send(clientSocket,finalResponse,msglen,0) == -1) {
                            perror("Error in sending\n");
                        }
                        printf("Peer %d: Final request is sent\n", nodeID);
                        remove_info_from_list(hashRequestInfoList, clientRequestInfo);
                        //freeInfo(clientRequestInfo);
                        //freeInfo(hashRequestInfo);
                        //free(finalResponse);
                        close(clientSocket);
                        FD_CLR(clientSocket, &master);
                        close(i);
                        FD_CLR(i, &master);
                    }

                }
            }
        }        
    }
    return 0;
                      
}
