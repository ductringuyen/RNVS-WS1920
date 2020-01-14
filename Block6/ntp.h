void rv_memcpy(void* dst, void* src, unsigned int len);

int bindToPort(char* portString);

double getTimeStamp(struct timespec clock);

unsigned char* createNTPRequest(double t1_unix);

void analizeTheResponse(unsigned char* response, double * T2, double * T3, float * rootDispersion);