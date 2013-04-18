#include <set>
#include <vector>

class CUncompressedTile {
	int tileSize, logTileSize, last;
	std::vector<int> xDiffs;
	std::set<int> yDiffs; // for transposed matrix

	void byteEncode(int val, unsigned char *buffer, int &startBuf);
	void deflateBytes(unsigned char *inBuffer, int ibLen, unsigned char *outBuffer, int &obLen);
public:
	CUncompressedTile();
	~CUncompressedTile();
	void addLink(int x, int y);
	void clear();
	void setSizes(int tile_size, int log_tile_size);
	bool isNotEmpty();
	int compress(int xTile, int yTile, unsigned char *buffer, int &startBuffer);
	int getSize();
};