#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include "uthash.h"
#include "hashing.h"
#include "dht.h"
//what is this
#define ACK 8
#define GET 4
#define SET 2
#define DEL 1
#define BACKLOG 10

#define LOOKUP 129
#define REPLY 130
#define FINAL 131
#define HASH 132

#define unknownPeer 0
#define thisPeer 1
#define nextPeer 2

#define YES 1
#define NO 0 


// For the Peers
unsigned int nodeID;
unsigned int prevID;
unsigned int nextID;

int nodeIP;
int nextIP;

unsigned int nodePort;
unsigned int prevPort;
unsigned int nextPort;

int clientSocket;  // Socket to the sender of the Hash Request   

int main(int argc, char** argv){

    if (argc != 10) {
        printf("Not enough arguments\n");
        exit(1);
    }

    // For the Select function 
    int requestType;           			// What kind of Connector, Clients or Peers?
    fd_set master;    					// master file descriptor list
    fd_set read_fds;  					// temp file descriptor list for select()
    int fdmax;        					// maximum file descriptor number 		

    
    // Peer and Neighbor Info 
    nodeID = atoi(argv[1]);
    prevID = atoi(argv[4]);
    nextID = atoi(argv[7]);
    
    nodePort = atoi(argv[3]);
    prevPort = atoi(argv[6]);
    nextPort = atoi(argv[9]);

    /*-------------------------------------------- GET PEER INFO --------------------------------------------------*/
    struct addrinfo hints, *servinfo;
    int status;
    
    memset(&hints, 0, sizeof hints);    	   // hints is empty 
    int listener, nextSocket, newSocketFD, clientSocket, firstPeerSocket, chosenPeerSocket;
    struct sockaddr_storage addrInfo;    	   // connector's addresponses Info
    socklen_t addrSize;

    hints.ai_family = AF_INET;        	   	   // IPv4
    hints.ai_socktype = SOCK_STREAM;           // Stream listener
    hints.ai_flags = AI_PASSIVE;               // Use my IP


    // Get Info of the actual peer   
    status = getaddrinfo(NULL, argv[3], &hints, &servinfo);
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
    
    struct sockaddr_in *ipv4 = (struct sockaddr_in*) servinfo->ai_addr;
    nodeIP = *(int*)(&ipv4->sin_addr); //////////// Where magic happen /////////////////
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

    FD_ZERO(&master);          // clear the master and temp sets
    FD_ZERO(&read_fds);	
    FD_SET(listener, &master); // add the listener to the master set
    fdmax = listener;		   // keep track of the biggest file descriptor

    
    while(1){

        read_fds = master;    // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(int i = 0; i <= fdmax; i++) {
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
                    int msglen = recv(i, firstByte, 1, 0);
                    if (msglen <= 0) {
                        perror("Error in receiving");
                        close(i);           
                        FD_CLR(i, &master); 
                    }
                    requestType = firstByteDecode(firstByte);                        
                                        
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

                        int hashValue = ringHashing(hashKey);
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
                            nextSocket = createConnection(argv[8],argv[9],&nextIP);
                        	unsigned char* hashRequest = createHashRequest(hashRequestInfo);
                        	int msglen = 7 + hashRequestInfo->keyLen + hashRequestInfo->valueLen;
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
                            //create and send LOOKUP Request
                            printf("Peer %d: my IP is %d\n", nodeID, nodeIP);
                            peerRequest = createPeerRequest(hashKey,nodeID,nodeIP,nodePort,LOOKUP);
                            nextSocket = createConnection(argv[8],argv[9],&nextIP);
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
                        int hashValue = ringHashing(hashID);
                        printf("Peer %d: hashValue is %d\n", nodeID, hashValue);
                        
                        // There won't be the case of thisPeer with LOOKUP
                        
                        if (checkPeer(nodeID,prevID,nextID,hashValue) == nextPeer) {
                            printf("Peer %d: my next pal %d is responsible for the request\n", nodeID,nextID);
                            unsigned char* hashID = malloc(2);
                            memcpy(hashID,peerRequest+1,2);
                            
                            int firstPeerIP;
                            memcpy(&firstPeerIP,peerRequest+5,4);
                            char ipString[INET_ADDRSTRLEN];
                            inet_ntop(AF_INET, &firstPeerIP, ipString, sizeof(ipString));

                            unsigned int firstPeerPort;
                            rv_memcpy(&firstPeerPort,peerRequest+9,2);
                            char portString[20];
                            itoa(firstPeerPort,portString);
                            
                            // Get the next IP
                            status = getaddrinfo(argv[8], argv[9], &hints, &servinfo);
                            if (status != 0) {
                                printf("getaddrinfo error: %s\n",gai_strerror(status));
                                exit(1);
                            }    
                            struct sockaddr_in *ipv4 = (struct sockaddr_in*) servinfo->ai_addr;
                            nextIP = *(int*)(&ipv4->sin_addr); //////////// Where magic happen /////////////////
                            freeaddrinfo(servinfo);
                            // Got the next IP

                            peerRequest = createPeerRequest(hashID,nextID,nextIP,nextPort,REPLY);
                            // Connect to the first peer
                            firstPeerSocket = createConnection(ipString,portString,NULL);

                            if (send(firstPeerSocket,peerRequest,11,0) == -1) {
                                perror("Error in sending\n");
                            }  
                        } else if (checkPeer(nodeID,prevID,nextID,hashValue) == unknownPeer) {
                            printf("Peer %d: I dunno but I'll ask my next pal %d\n", nodeID,nextID);
                            nextSocket = createConnection(argv[8],argv[9],&nextIP);
                            if (send(nextSocket,peerRequest,11,0) == -1) {
                                perror("Error in sending\n");
                            }
                        } 
                    } else if (requestType == REPLY) {
                        //get full request
                        printf("Peer %d: received a REPLY Request\n", nodeID);
                        unsigned char* peerRequest;
                        peerRequest = getPeerRequest(i,firstByte);

                        int chosenPeerIP;
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
                        int msglen = 7 + hashRequestInfo->keyLen + hashRequestInfo->valueLen;
                        if (send(chosenPeerSocket,hashRequest,msglen,0) == -1) {
                            perror("Error in sending\n");
                        }

                        FD_SET(chosenPeerSocket, &master); // add to master set
                        if (chosenPeerSocket > fdmax){     // keep track of the max
                            fdmax = chosenPeerSocket;
                        }

                    } else if (requestType == FINAL) {
                    	hash_request_info* hashRequestInfo;
                        printf("Peer %d: received a FINAL Request\n", nodeID);
                        hashRequestInfo = getHashRequestInfo(i,firstByte);
                        unsigned char* finalResponse = createHashRequest(hashRequestInfo);
                        hash_request_info* clientRequestInfo;
                        clientRequestInfo = getClientRequestInfo(hashRequestInfoList, hashRequestInfo, i);
                        clientSocket = clientRequestInfo->callerSocket;
                        printf("Peer %d: Got the final Response\n",nodeID);
                        int msglen = hashRequestInfo->keyLen + hashRequestInfo->valueLen +7;
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
