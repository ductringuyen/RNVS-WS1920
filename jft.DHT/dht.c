#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/mman.h>
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

#define unknown 100

unsigned int ringHashing(unsigned char* key) {
  unsigned int hashValue = 0;
  for (unsigned int i = 0; i < 2; i++) {
    hashValue += (unsigned int)(key[i]%80);
  }
  return hashValue;
};

unsigned int checkPeer(unsigned int nodeID, unsigned int prevID, unsigned int nextID, unsigned int hashValue) {
	if ((hashValue <= nodeID && hashValue > prevID) || (prevID > nodeID && hashValue > prevID) || (prevID > nodeID && hashValue < nodeID) || (nextID == unknown)) {
		return thisPeer;
	} else if ((hashValue <= nextID && hashValue > nodeID) || (nextID < nodeID && hashValue > nodeID) || (nextID < nodeID && hashValue < nextID)) {
		return nextPeer;
	}
	return unknownPeer;
}

unsigned int checkJoinPeer(unsigned int nodeID, unsigned int prevID, unsigned int nextID, unsigned int callerID) {
  // 1 Peer in Ring
  if (nextID == unknown) return thisPeer;
  // 2 Peers in Ring
  if (nextID == prevID && nodeID < nextID && callerID > nextID) return thisPeer;
  // 3 or more Peers in Ring           
  if (nodeID > callerID && prevID < callerID) return thisPeer;
  if (nodeID < callerID && prevID > nodeID && nextID != prevID) return thisPeer;
  return unknownPeer;
}

unsigned int firstByteDecode(unsigned char* firstByte) {
	if (*firstByte < 5){
		return HASH;
	} else if (*firstByte < 13 && *firstByte > 8) {
    return FINAL;  
  } else {
    return *firstByte;  
  }
}

void rv_memcpy(void* dst, void* src, unsigned int len) {
  unsigned char* dstByte = (unsigned char*) dst;
  unsigned char* srcByte = (unsigned char*) src;
  for (unsigned int i = 0; i < len; i++) {
    dstByte[i] = srcByte[len-1-i];
  }
}

void hashHeaderAnalize(unsigned char* header, unsigned int* keyLen, unsigned int* valueLen) {
	*keyLen = (header[0]<<8) + header[1];
	*valueLen = (header[2]<<24) + (header[3]<<16) + (header[4]<<8) + header[5];
}

hash_request_info* getHashRequestInfo(unsigned int socketfd, unsigned char* firstByte){
    hash_request_info* info  = malloc(sizeof(hash_request_info*));
	  unsigned char* header;
	  unsigned char* data;
	  unsigned int opt;
    unsigned int keyLen;
    unsigned int valueLen;
    unsigned int written = 0;
    unsigned int msglen = 0;
  
    info->callerSocket = socketfd;
    info->opt = *firstByte;
    opt = *firstByte;
	  // Get the Header
    header = malloc(6);	
    msglen = recv(socketfd, header, 6, 0);
    if (msglen == -1) {
        perror("Error in receiving\n");
        exit(1);
    }
    hashHeaderAnalize(header, &keyLen, &valueLen);
    info->keyLen = keyLen;
    info->valueLen = valueLen;
    // Get the full request
    data = malloc(keyLen + valueLen);
    while(1) {
       	msglen = recv(socketfd,data + written, 512, 0);
       	if (msglen == -1) {
       	  continue;
       	}
        written += msglen;
        if (written == keyLen + valueLen) break;	
    }
    if (keyLen > 0) {
      info->key = malloc(keyLen);
      memcpy(info->key,data,keyLen);      
    }
    if (valueLen > 0) {
      info->value = malloc(valueLen);
      memcpy(info->value,data + keyLen,valueLen);
    }
    info->next = NULL;
    return info;
}

unsigned char* createHashRequest(hash_request_info* hashRequestInfo) {
    unsigned char* request = malloc(7 + hashRequestInfo->keyLen + hashRequestInfo->valueLen);
    memcpy(request,&(hashRequestInfo->opt),1);
    rv_memcpy(request+1,&(hashRequestInfo->keyLen),2);
    rv_memcpy(request+3,&(hashRequestInfo->valueLen),4);
    memcpy(request+7,hashRequestInfo->key,hashRequestInfo->keyLen);
    memcpy(request+7+hashRequestInfo->keyLen,hashRequestInfo->value,hashRequestInfo->valueLen);
    return request;
}

unsigned char* peerHashing(hashable** hTab, hash_request_info* hashRequestInfo, unsigned int* responseLen) {
	unsigned char* response;
	unsigned int resLen;

  unsigned int opt = hashRequestInfo->opt;
  unsigned int keyLen = hashRequestInfo->keyLen;
  unsigned int valueLen = hashRequestInfo->valueLen;
  unsigned char* key = malloc(keyLen);
  unsigned char* value = malloc(valueLen);
  memcpy(key,hashRequestInfo->key,keyLen);
  memcpy(value,hashRequestInfo->value,valueLen);

	if (opt == SET) {
            printf("SET\n");
            resLen = 7;
           	response = calloc(resLen,1);
            response[0] = ACK + SET;			     // set Ack bit of the response
           	set(hTab, key, value, keyLen, valueLen);
        } 
    else if (opt == GET) {
        printf("GET\n");
        hashable *hashElem = get(hTab, key, keyLen);
       	if (hashElem == NULL) {
            resLen = 7;
            response = calloc(resLen,1);
            response[0] = ACK + GET;            // set Ack bit of the response
        } else {
            valueLen = hashElem->valueLen;
            resLen = 7 + hashElem->keyLen + hashElem->valueLen;
            response = malloc(resLen);
            response[0] = ACK + GET;            // set Ack bit of the response
            rv_memcpy(response + 1, &keyLen, 2);
            rv_memcpy(response + 3, &valueLen, 4);
            memcpy(response + 7,hashElem->hashKey,hashElem->keyLen);
            memcpy(response + 7 + hashElem->keyLen,hashElem->hashValue,hashElem->valueLen);
           }
        }
    else if (opt == DEL) {
           printf("DELETE\n"); 
           resLen = 7;
           response = (unsigned char*) calloc(resLen,1);
           response[0] = ACK + DEL;			// set Ack bit of the response
           delete(hTab, key, keyLen);
    }
    *responseLen = resLen;
    return response;
}

unsigned char* createPeerRequest(unsigned char* hashID, unsigned int nodeID, unsigned int nodeIP, unsigned int nodePort, unsigned int operation) {
	unsigned char *request = malloc(11);
	*request = operation;
	memcpy(request+1,hashID,2);
	rv_memcpy(request+3,&nodeID,2);
	memcpy(request+5,&nodeIP,4);
	rv_memcpy(request+9,&nodePort,2);
	return request;
}

unsigned char* getPeerRequest(unsigned int socketfd, unsigned char* firstByte) {
	unsigned char* request = malloc(11);
	unsigned char* data = malloc(10);
	unsigned int msglen = recv(socketfd,data, 10, 0);
    if (msglen == -1) {
       	perror("Error in receiving\n");
       	exit(0);
    }
    memcpy(request,firstByte,1);
    memcpy(request+1,data,10);
    free(data);
    return request;
}

char* itoa(unsigned int num, char *str) {
  sprintf(str, "%d", num);
  return str;
}

unsigned int createConnection(char* addr, char* port, unsigned int* IP) {
    struct addrinfo hints, *servinfo;
    unsigned int status;
    
    memset(&hints, 0, sizeof hints);          // hints is empty 
    unsigned int Socket;
    struct sockaddr_storage addrInfo;         // connector's addresss Info
    socklen_t addrSize;

    hints.ai_family = AF_INET;                 // IPv4
    hints.ai_socktype = SOCK_STREAM;           // Stream listener
    hints.ai_flags = AI_PASSIVE;               // Use my IP

    status = getaddrinfo(addr, port, &hints, &servinfo);
    if (status != 0) {
        printf("getaddrinfo error: %s\n",gai_strerror(status));
        exit(1);
    }

    while(1) {
        Socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
        if(Socket == -1) {
            perror("Failed to create a Socket\n");
            continue;
        }
        // IF SOCKET IS CREATED, TRY TO CONNECT TO THE SERVER
        if (connect(Socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
            close(Socket);
            //perror("Problems with creating connection\n");
            continue;
        }
        break; // sucessfully connect
    }
    // Get IP as a number
    if (IP != NULL) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in*) servinfo->ai_addr;
      *IP = *(unsigned int*)(&ipv4->sin_addr);         //////////// Where magic happen /////////////////
    }
    freeaddrinfo(servinfo);
    return Socket;
}
        
unsigned int put_in_the_list(hash_request_info_list* list, hash_request_info* hashRequestInfo) {
    if (list->head == NULL) {
        list->head = hashRequestInfo;
        return 0;
    }
    hash_request_info* tmp;
    tmp = list->head;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    tmp->next = hashRequestInfo;
    return 0;
}

unsigned int remove_info_from_list(hash_request_info_list* list, hash_request_info* hashRequestInfo) {
    hash_request_info* tmp;
    tmp = list->head;
    if (tmp == hashRequestInfo) {
      list->head = hashRequestInfo->next;
      hashRequestInfo->next = NULL;
      return 0;
    }
    while (tmp->next != hashRequestInfo) {
        tmp = tmp->next;
    }
    tmp->next = hashRequestInfo->next;
    hashRequestInfo->next = NULL;
    return 0;
}

hash_request_info* findHashRequestInfo(hash_request_info_list* list, unsigned char* hashID) {
    hash_request_info* tmp;
    tmp = list->head;
    while (tmp != NULL) {
        if (tmp->keyLen == 1 && hashID[1] == 0) {
            if (memcmp(tmp->key,hashID,1) == 0) {
                return tmp;
            }
        }
        if (memcmp(tmp->key,hashID,2) == 0) {
            return tmp;            
        }
        tmp = tmp->next;
    }
    return NULL;
}

void freeInfo(hash_request_info* hashRequestInfo) {
    if(hashRequestInfo->keyLen != 0) free(hashRequestInfo->key);
    if(hashRequestInfo->valueLen != 0) free(hashRequestInfo->value);
    free(hashRequestInfo);
}

hash_request_info* getClientRequestInfo(hash_request_info_list* list, hash_request_info* hashRequestInfo, unsigned int socketfd) {
    hash_request_info* tmp;
    tmp = list->head;
    while (tmp != NULL) {
        if (hashRequestInfo->opt - tmp->opt == ACK && tmp->finalSocket == socketfd) {
            return tmp;
        }
        tmp = tmp->next;
    }

    return NULL;   
}