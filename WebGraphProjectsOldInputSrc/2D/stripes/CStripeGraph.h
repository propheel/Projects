#include "CUncompressedTile.h"
#include "CCompressedTile.h"

class CStripeGraph {
	unsigned char *graphBuffer;
	int *xOffsets, *yOffsets, graphBufferStart, tilesAmount, tilesInARow, logTileSize, tileSize, nodesAmount;
	int *xFirstOffsets, *yFirstOffsets, *outArr;
	CCompressedTile tileForUncompress;
public:
	CStripeGraph();
	~CStripeGraph();
	bool loadFromTextFile(const char *textFileName, const int tile_size);
	bool loadFromFile(const char *graphFileName, const char *offsetsFileName, const int tile_size);
	void saveToFile(const char *graphFileName, const char *offsetsFileName);
	void clear();
	int successorArray(int node, int **sucArrPtr);
	int predecessorArray(int node, int **preArrPtr);
	int size();
};