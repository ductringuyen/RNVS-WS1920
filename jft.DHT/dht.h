typedef struct s_hashRequest {
	int callerSocket;
	int finalSocket;
	unsigned int opt;				
    unsigned int keyLen;			
    unsigned int valueLen;			
    unsigned char* key;				
    unsigned char* value;
    struct s_hashRequest* next;
} hash_request_info;

typedef struct s_hash_request_list {
    hash_request_info* head;
} hash_request_info_list;

int ringHashing(unsigned char* key);

int checkPeer(int nodeID, int prevID, int nextID, int hashValue);

int firstByteDecode(unsigned char* firstByte);

void rv_memcpy(void* dst, void* src, unsigned int len);

void hashHeaderAnalize(unsigned char* header, unsigned int* keyLen, unsigned int* valueLen);

hash_request_info* getHashRequestInfo(int socketfd, unsigned char* firstByte);

unsigned char* createHashRequest(hash_request_info* hashRequestInfo);

unsigned char* peerHashing(hashable** hTab, hash_request_info* hashRequestInfo, unsigned int* responseLen);

unsigned char* createPeerRequest(unsigned char* hashID, unsigned int nodeID, unsigned int nodeIP, unsigned int nodePort, int operation);

unsigned char* getPeerRequest(int socketfd, unsigned char* firstByte);

char* itoa(int num, char *str);

int createConnection(char* addr, char* port, int* IP);

int put_in_the_list(hash_request_info_list* list, hash_request_info* hashRequestInfo);

int remove_info_from_list(hash_request_info_list* list, hash_request_info* hashRequestInfo);

hash_request_info* findHashRequestInfo(hash_request_info_list* list, unsigned char* hashID);

void freeInfo(hash_request_info* hashRequestInfo);

hash_request_info* getClientRequestInfo(hash_request_info_list* list, hash_request_info* hashRequestInfo, int socketfd);



