#include <set>
#include <vector>

class CUncompressedTile {
	unsigned int logTileSize;
	long long last;
	std::vector<long long> xDiffs;
	std::set<long long> yDiffs; // for transposed matrix

	void byteEncode(long long val, unsigned char *buffer, int &startBuf);
	void deflateBytes(unsigned char *inBuffer, int ibLen, unsigned char *outBuffer, int &obLen);
public:
	CUncompressedTile();
	~CUncompressedTile();
	void addLink(int x, int y);
	void clear();
	void setSizes(unsigned int log_tile_size);
	bool isNotEmpty();
	unsigned int compress(unsigned char *buffer, unsigned int &startBuffer);
	unsigned int getSize();
};