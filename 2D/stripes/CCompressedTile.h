#ifndef __CCOMPRESSED_TILE__
#define __CCOMPRESSED_TILE__

class CCompressedTile {
	int logTileSize, tileSize, len;
	int y, x, xStripOffset, yStripOffset, tmpXmask, tmpYmask;
	int offsetVal, nextOffsetVal, length, tmpStart, lengthOffset;
	int last, tmp, tmpLen, defaultTmpLen, lastX, lastY, tmpLast;
	unsigned char *tmpB, *buffer;
	int *outArr, interestedFrom, interestedTo, tmpX, tmpY;
	int nextInt(unsigned char *buf, int &start);
	void inflateBytes(unsigned char *inBuffer, int ibLen, unsigned char *outBuffer, int &obLen);
public:
	CCompressedTile();
	~CCompressedTile();
	void setParams(int tile_size, int log_tile_size, unsigned char *buffer, int *outArr);
	void setX(int xInTile);
	void setY(int yInTile);
	void decompressX(int offset, int nextOffset);
	void decompressY(int offset, int nextOffset);
	int getLen();
};

#endif