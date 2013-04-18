#include "CUncompressedTile.h"
#include <zlib.h>

CUncompressedTile::CUncompressedTile() {
	tileSize = stripSize = 0;
	xStrip = yStrip = 0;
}

CUncompressedTile::~CUncompressedTile() {
	delete[] xStrip;
	delete[] yStrip;
}

void CUncompressedTile::setSizes(int tile_size, int log_tile_size, int strip_size) {

	tileSize = tile_size;
	logTileSize = log_tile_size;
	stripSize = strip_size;

	int bytesPerStrip = (stripSize+7) >> 3;
	delete[] xStrip;
	delete[] yStrip;
	xStrip = new unsigned char[bytesPerStrip];
	yStrip = new unsigned char[bytesPerStrip];

	clear();
}

void CUncompressedTile::clear() {
	int bytesPerStrip = (stripSize+7) >> 3;
	for(int i=0; i<bytesPerStrip; i++)
		xStrip[i] = yStrip[i] = 0;

	xDiffs.clear();
	yDiffs.clear();
	last = -1;
}

void CUncompressedTile::addLink(int x, int y) {

	xDiffs.push_back((y << logTileSize) + x - last);
	last = (y << logTileSize) + x;

	yDiffs.insert((x << logTileSize) + y);

	int tmpX = (x * stripSize) >> logTileSize; // mark columns (that they contain something)
	xStrip[tmpX >> 3] |= 1 << (tmpX & 7);

	int tmpY = (y * stripSize) >> logTileSize; // mark rows
	yStrip[tmpY >> 3] |= 1 << (tmpY & 7);
	
}

bool CUncompressedTile::isNotEmpty() {
	return (xDiffs.size() != 0);
}

int CUncompressedTile::compress(int xTile, int yTile, unsigned char *buffer, int &startBuf) {
	// try four methods and choose the best one

	//method 1: byte encode x diffs:
	unsigned char *buf1 = new unsigned char[xDiffs.size() * 3 + 1];
	int startBuf1 = 0;
	for(auto it=xDiffs.begin(); it!=xDiffs.end(); it++)
		byteEncode(*it, buf1, startBuf1);
	byteEncode(0, buf1, startBuf1);

	//method 2: zip byte encode x diffs
	int startBuf2 = (int)xDiffs.size() * 5 + 1;
	unsigned char *buf2 = new unsigned char[startBuf2];
	deflateBytes(buf1, startBuf1, buf2, startBuf2);

	//method 3: byte encode y diffs (x transposed)
	unsigned char *buf3 = new unsigned char[yDiffs.size() * 3 + 1];
	int startBuf3 = 0;
	last = -1;
	for(auto it=yDiffs.begin(); it!=yDiffs.end(); it++) {
		byteEncode((*it)-last, buf3, startBuf3);
		last = *it;
	}
	byteEncode(0, buf3, startBuf3);

	//method 4: zip byte encode y diffs (x transposed)
	int startBuf4 = (int)yDiffs.size() * 5 + 1;
	unsigned char *buf4 = new unsigned char[startBuf4];
	deflateBytes(buf3, startBuf3, buf4, startBuf4);

	//choose method
	int ret = startBuf;
	if(startBuf1 <= startBuf2)
		if(startBuf3 <= startBuf4)
			if(startBuf1 <= startBuf3) {
				// choose 1
				memcpy(buffer + startBuf, buf1, startBuf1);
				startBuf += startBuf1;
			} else {
				// choose 3
				memcpy(buffer + startBuf, buf3, startBuf3);
				startBuf += startBuf3;
				ret |= 2 << 30; //(2 << 30);
			}
		else
			if(startBuf1 <= startBuf4) {
				// choose 1
				memcpy(buffer + startBuf, buf1, startBuf1);
				startBuf += startBuf1;
			} else {
				// choose 4
				memcpy(buffer + startBuf, buf4, startBuf4);
				startBuf += startBuf4;
				ret |= 3 << 30;
			}
	else
		if(startBuf3 <= startBuf4)
			if(startBuf2 <= startBuf3) {
				// choose 2
				memcpy(buffer + startBuf, buf2, startBuf2);
				startBuf += startBuf2;
				ret |= 1 << 30;
			} else {
				// choose 3
				memcpy(buffer + startBuf, buf3, startBuf3);
				startBuf += startBuf3;
				ret |= 2 << 30;
			}
		else
			if(startBuf2 <= startBuf4) {
				// choose 2
				memcpy(buffer + startBuf, buf2, startBuf2);
				startBuf += startBuf2;
				ret |= 1 << 30;
			} else {
				// choose 4
				memcpy(buffer + startBuf, buf4, startBuf4);
				startBuf += startBuf4;
				ret |= 3 << 30;
			}

	delete[] buf1;
	delete[] buf2;
	delete[] buf3;
	delete[] buf4;

	// save xStrip and yStrip as well
	int bytesPerStrip = (stripSize+7) >> 3;
	memcpy(buffer + startBuf, xStrip, bytesPerStrip);
	startBuf += bytesPerStrip;
	memcpy(buffer + startBuf, yStrip, bytesPerStrip);
	startBuf += bytesPerStrip;

	// and x and y of a tile
	buffer[startBuf++] = xTile & 255;
	buffer[startBuf++] = (xTile >> 8) & 255;
	buffer[startBuf++] = xTile >> 16;

	buffer[startBuf++] = yTile & 255;
	buffer[startBuf++] = (yTile >> 8) & 255;
	buffer[startBuf++] = yTile >> 16;

	return ret;
}


void CUncompressedTile::deflateBytes(unsigned char *inBuffer, int ibLen, unsigned char *outBuffer, int &obLen) {
		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
//		int ret = deflateInit(&strm, 9); // compression level 9 (0-9, -1 auto)
		int ret = deflateInit2(&strm, 9, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
/*		if(ret != Z_OK) {
			printf("Error: DeflateInit failed.\n");
			return;
		}
*/		strm.avail_in = (unsigned int)ibLen;
		strm.next_in = inBuffer;
		strm.avail_out = (unsigned int)obLen;
		strm.next_out = outBuffer;
		ret = deflate(&strm, Z_FINISH); // we make it only once, so Z_FLUSH
		obLen -= strm.avail_out;
		deflateEnd(&strm);
}


void CUncompressedTile::byteEncode(int val, unsigned char *buffer, int &startBuf) { // max 3 bytes
	val <<= 2;
	if(val < 256) 
		buffer[startBuf++] = (unsigned char)val;
	else if(val < 65536) {
		val++;
		buffer[startBuf++] = val & 255;
		buffer[startBuf++] = val >> 8;
	} else {
		val += 2;
		buffer[startBuf++] = val & 255;
		buffer[startBuf++] = (val >> 8) & 255;
		buffer[startBuf++] = val >> 16;
	}
}