#include "CUncompressedTile.h"
#include <zlib.h>

CUncompressedTile::CUncompressedTile() {
	logTileSize = 0;
}

CUncompressedTile::~CUncompressedTile() {
}

void CUncompressedTile::setSizes(unsigned int log_tile_size) {
	logTileSize = log_tile_size;
	clear();
}

void CUncompressedTile::clear() {
	xDiffs.clear();
	yDiffs.clear();
	last = -1;
}

void CUncompressedTile::addLink(int x, int y) {

	xDiffs.push_back(((long long)y << logTileSize) + (long long)x - last);
	last = ((long long)y << logTileSize) + (long long)x;

	yDiffs.insert(((long long)x << logTileSize) + (long long)y);

}

bool CUncompressedTile::isNotEmpty() {
	return (xDiffs.size() != 0);
}

unsigned int CUncompressedTile::compress(unsigned char *buffer, unsigned int &startBuf) {

	unsigned int ret = startBuf;

	// if there is only one value (tileSize == 1 and logTileSize == 0), just write 0
	if(logTileSize == 0) {
		buffer[startBuf++] = 0;
		return ret;
	}
	
	// try four methods and choose the best one

	//method 1: byte encode x diffs:
	unsigned char *buf1 = new unsigned char[(xDiffs.size() << 3) + 1];
	int startBuf1 = 0;
	for(auto it=xDiffs.begin(); it!=xDiffs.end(); it++)
		byteEncode(*it, buf1, startBuf1);
	byteEncode(0, buf1, startBuf1);

	//method 2: zip byte encode x diffs
	int startBuf2 = startBuf1;
	unsigned char *buf2 = new unsigned char[startBuf2];
	deflateBytes(buf1, startBuf1, buf2, startBuf2);

	//method 3: byte encode y diffs (x transposed)
	unsigned char *buf3 = new unsigned char[(yDiffs.size() << 3) + 1];
	int startBuf3 = 0;
	last = -1;
	for(auto it=yDiffs.begin(); it!=yDiffs.end(); it++) {
		byteEncode((*it)-last, buf3, startBuf3);
		last = *it;
	}
	byteEncode(0, buf3, startBuf3);

	//method 4: zip byte encode y diffs (x transposed)
	int startBuf4 = startBuf3;
	unsigned char *buf4 = new unsigned char[startBuf4];
	deflateBytes(buf3, startBuf3, buf4, startBuf4);

	//choose method
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
				ret |= (2 << 30);
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
				ret |= (3 << 30);
			}
	else
		if(startBuf3 <= startBuf4)
			if(startBuf2 <= startBuf3) {
				// choose 2
				memcpy(buffer + startBuf, buf2, startBuf2);
				startBuf += startBuf2;
				ret |= (1 << 30);
			} else {
				// choose 3
				memcpy(buffer + startBuf, buf3, startBuf3);
				startBuf += startBuf3;
				ret |= (2 << 30);
			}
		else
			if(startBuf2 <= startBuf4) {
				// choose 2
				memcpy(buffer + startBuf, buf2, startBuf2);
				startBuf += startBuf2;
				ret |= (1 << 30);
			} else {
				// choose 4
				memcpy(buffer + startBuf, buf4, startBuf4);
				startBuf += startBuf4;
				ret |= (3 << 30);
			}

	delete[] buf1, buf2, buf3, buf4;

	clear();

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


void CUncompressedTile::byteEncode(long long val, unsigned char *buffer, int &startBuf) {
	val <<= 2;
	if(val < (1 << 6)) 
		buffer[startBuf++] = (unsigned char)val;
	else if(val < (1 << 14)) {
		val |= 1;
		memcpy(buffer+startBuf, &val, 2);
		startBuf += 2;
	} else if(val < (1 << 30)) {
		val |= 2;
		memcpy(buffer+startBuf, &val, 4);
		startBuf += 4;
	} else {
		val |= 3;
		memcpy(buffer+startBuf, &val, 8);
		startBuf += 8;
	}
}

unsigned int CUncompressedTile::getSize() {
	return xDiffs.size();
}