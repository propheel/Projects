class CompressedBuffer {
	unsigned char *buffer;
	long long offset, size;
public:
	CompressedBuffer();
	~CompressedBuffer();
	void reset(long long initSize);
	void compressBytes(unsigned char *inputB, long long length);
	long long getOffset();
	void setOffset(long long off);
	unsigned char *getBuffer();
	bool uncompressBytes(unsigned char *outputB, long long &outSize, long long offset, long long len);
};