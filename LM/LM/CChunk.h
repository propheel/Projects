class CChunk {
	unsigned char **flagsBitArrays;
	unsigned char *uncomprBuffer;
	int *residArray, h, bitsPerH, bytesPerH, numResid, b, type;

	int byteEncode(int val, unsigned char *buffer);
	int byteEncode1bit(int val, unsigned char *buffer);
	int compressBytes(unsigned char *inputB, int length, unsigned char *outBuffer, int bufferBytesLeft);
	void calculateBits(int maxNodeNr, const int h);
public:
	CChunk(int maxNodeNr, const int h);
	CChunk(unsigned char *buffer, int length, int maxNodeNr, const int h, int maxUncomprSize);
	~CChunk();
	void prepare(int maxNodeNr, const int h);
	void createEdge(int x, int y);
	int process(unsigned char *buffer, int bufferBytesLeft, int *maxResidListSize, int *maxUncomprSize);
	void getSucc(const int row, int **sucArr, int *len, int maxResidListSize);
};