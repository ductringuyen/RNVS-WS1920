typedef struct s_hashable
{
	void* hashKey;
	void* hashValue;
	int keyLen;
	int valueLen;
	UT_hash_handle hh;
} hashable ;

void* keyProcessing(hashable* hTab, void* key, int kl);

hashable* get(hashable** hTab, void* key, int kl);

void set(hashable** hTab, void* key, void* value, int kl, int vl); 

void delete(hashable** hTab, void* key, int kl);






