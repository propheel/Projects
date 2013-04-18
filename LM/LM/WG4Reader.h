#include "definitions.h"

class WG4Reader {
	FILE *f;
	int errorNr, nodes, bufferPos, bufferSize;
	long long edges;
	unsigned char *buffer;

	void takeFromBuffer(void *destBuff, int howManyBytes);
	void checkIfThereAreBytesInBuffer(int howManyBytes); // checks and refills buffer
public:
	WG4Reader();
	WG4Reader(const char *inputFileName);
	~WG4Reader();
	void open(const char *inputFileName);
	bool error();
	int numNodes();
	long long numEdges();
	void getNumSucc(int *succNr); // gets number of successors and moves file pointer
	void getNextSucc(int *succNodeNr); // gets number of successor node and moves file pointer
};