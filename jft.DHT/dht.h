typedef struct s_hashRequest {
	unsigned int callerSocket;
	unsigned int finalSocket;
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

typedef struct s_fingerTable_elem {
	unsigned int start;
	unsigned int peerID;
	unsigned int peerIP;
	unsigned int peerPort;
} fingerTable_elem;

unsigned int ringHashing(unsigned char* key);

unsigned int checkPeer(unsigned int nodeID, unsigned int prevID, unsigned int nextID, unsigned int hashValue);

unsigned int checkJoinPeer(unsigned int nodeID, unsigned int prevID, unsigned int nextID, unsigned int callerID);

unsigned int firstByteDecode(unsigned char* firstByte);

void rv_memcpy(void* dst, void* src, unsigned int len);

void hashHeaderAnalize(unsigned char* header, unsigned int* keyLen, unsigned int* valueLen);

hash_request_info* getHashRequestInfo(unsigned int socketfd, unsigned char* firstByte);

unsigned char* createHashRequest(hash_request_info* hashRequestInfo);

unsigned char* peerHashing(hashable** hTab, hash_request_info* hashRequestInfo, unsigned int* responseLen);

unsigned char* createPeerRequest(unsigned char* hashID, unsigned int nodeID, unsigned int nodeIP, unsigned int nodePort, unsigned int operation);

unsigned char* getPeerRequest(unsigned int socketfd, unsigned char* firstByte);

char* itoa(unsigned int num, char *str);

unsigned int createConnection(char* addr, char* port, unsigned int* IP);

unsigned int put_in_the_list(hash_request_info_list* list, hash_request_info* hashRequestInfo);

unsigned int remove_info_from_list(hash_request_info_list* list, hash_request_info* hashRequestInfo);

hash_request_info* findHashRequestInfo(hash_request_info_list* list, unsigned char* hashID);

void freeInfo(hash_request_info* hashRequestInfo);

hash_request_info* getClientRequestInfo(hash_request_info_list* list, hash_request_info* hashRequestInfo, unsigned int socketfd);

unsigned int exponential_of_two(unsigned int n);

unsigned int check_finger_table_input(unsigned int ft_input, unsigned int nodeID);

int finger_table_lookup(unsigned int hashValue, fingerTable_elem **ft_Elem);
