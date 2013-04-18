#include "CompressedBuffer.h"

class LMGraph {
	CompressedBuffer compressedGraph;
	long long *offsets, *outArr;
	long long lineNum, offsetsAmount, maxListLength, maxResidListLength, maxUncomprSize;
	int h, b, bitsPerH, bytesPerH;
	void calculateBits();
	unsigned char *bData;
public:
	LMGraph();
	~LMGraph();
	bool loadFromTextFile(const char *fileName, const int h);
	bool saveToFile(const char *graphFileName, const char *offsetFileName);
	bool loadFromFile(const char *graphFileName, const char *offsetFileName, const int h);
	bool saveToTextFile(const char *fileName);
	void printStats();
	long long * successorArray(long long row, long long &len);
	long long getNodesAmount();
	long long getMaxResidues();
	long long getLength();
};