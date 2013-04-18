#ifndef __CCOMPRESSED_TILE__
#define __CCOMPRESSED_TILE__

class CCompressedTile {
	unsigned int *len, maxTileElements;
	int *sucArr;
	unsigned char *buffer, *tmpB;

	long long nextInt(unsigned char *buf, unsigned int &start);
	void inflateBytes(unsigned char *inBuffer, int ibLen, unsigned char *outBuffer, int &obLen);
public:
	CCompressedTile();
	~CCompressedTile();
	void setParams(unsigned char *buffer, int *sucArr, unsigned int *len, unsigned int max_tile_elements);
	void decompressX(unsigned int logTileSize, int offX, int y, unsigned int offset, unsigned int nextOffset);
};

#endif