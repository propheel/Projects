#include "CUncompressedTile.h"
#include "CCompressedTile.h"

class DSGraph {
	CUncompressedTile *tiles;
	CCompressedTile tileForSuc;
	unsigned char *graphBuffer;
	int tileAmount, nodesAmount, level, maxListSize, maxTileElements, *sucArr;
	unsigned int graphBufferStart, *offsets, sucLen;
	int getTileNr(int x, int y, int *oX, int *oY);
public:
	DSGraph();
	~DSGraph();
	void loadFromTextFile(const char *textFileName);
	void loadFromFile(const char *graphFileName, const char *offsetsFileName);
	void saveToFile(const char *graphFileName, const char *offsetsFileName);
	unsigned int successorArray(int row, int **sucArrPtr);
};