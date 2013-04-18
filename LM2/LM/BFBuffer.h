class BFBuffer {
	unsigned char *buffer;
	long long start, size;
	int b;
public:
	BFBuffer(long long init, int bDiff);
	~BFBuffer();
	void byteEncode(long long val);
	unsigned char *getFlagBuffer();
	unsigned char *getBuffer();
	long long getListLength();
};