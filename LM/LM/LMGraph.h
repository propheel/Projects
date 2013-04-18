class LMGraph {
	bool loaded;
	unsigned char *graphBuffer;
	unsigned int graphSize;
	int numNodes, maxSuccListLength, graphPos, offsetsCnt, maxResidListLength, maxUncomprSize, h;
	unsigned int *offsets;
	long long numEdges;

	void resizeBuffer();
public:
	LMGraph();
	~LMGraph();
	void loadFromTextFile(const char *inputFileName, const int h); // load graph from wg4 file
	void saveToFiles(const char *graphFileName, const char *offsetsFileName); // save compressed graph
	void unLoad(); // release graph from memory
	void loadFromFiles(const char *graphFileName, const char *offsetsFileName, const int h); // load compressed graph
	void successorArray(const int row, int **sucArr, int *len); // retrieve all successors from row
	unsigned int getSize();
};

/*

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

*/