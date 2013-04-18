class BFBuffer {
	unsigned char *buffer;
	long long start, size, *offsets;
	int b, offCnt;
public:
	BFBuffer(int bDiff);
	~BFBuffer();
	void byteEncode(long long val);
	void addOffset(long long offset);
	void addOffsetsAtEnd();
	void clear();
	void resize();
	unsigned char *getFlagBuffer(long long space);
	unsigned char *getBuffer();
	long long getListLength();
};