#include "CCompressedTile.h"
#include <cstdio>
#include <string>
#include <zlib.h>

CCompressedTile::CCompressedTile() {
	tmpB = 0;
}

CCompressedTile::~CCompressedTile() {
	delete[] tmpB;
}

void CCompressedTile::setParams(unsigned char *buffer, int *sucArr, unsigned int *len, unsigned int max_tile_elements) {
	this->buffer = buffer;
	this->sucArr = sucArr;
	this->len = len;
	maxTileElements = max_tile_elements << 2;
	tmpB = new unsigned char[maxTileElements];
}

void CCompressedTile::decompressX(unsigned int logTileSize, int offX, int y, unsigned int offset, unsigned int nextOffset) {
	unsigned int offsetVal = offset & ((1 << 30)-1);
	unsigned int nextOffsetVal = nextOffset & ((1 << 30)-1);
	unsigned int length = nextOffsetVal - offsetVal;
	if(length == 0)
		return;
	if(logTileSize == 0) {
		sucArr[(*len)++] = offX;
		return;
	}
	long long last = -1;
	long long interestedFrom = (long long)y << logTileSize;
	long long interestedTo = interestedFrom + (1LL << logTileSize);
	long long tmp;
	if((offset & (1 << 31)) == 0) { // normal, not transposed
		if((offset & (1 << 30)) == 0) { // no zip
			tmp = nextInt(buffer, offsetVal);
			while(tmp != 0) {
				last += tmp;
				if(last >= interestedFrom)
					if(last < interestedTo)
						sucArr[(*len)++] = offX + (int)(last - interestedFrom);
					else
						break;
				tmp = nextInt(buffer, offsetVal);
			}
		} else { // zipped
			int tmpLen = maxTileElements;
			inflateBytes(buffer+offsetVal, length, tmpB, tmpLen);
			unsigned int tmpStart = 0;
			tmp = nextInt(tmpB, tmpStart);
			while(tmp != 0) {
				last += tmp;
				if(last >= interestedFrom)
					if(last < interestedTo)
						sucArr[(*len)++] = offX + (int)(last - interestedFrom);
					else
						break;
				tmp = nextInt(tmpB, tmpStart);
			}
		}
	} else { // transposed
		if((offset & (1 << 30)) == 0) { // no zip
			last = -1;
			tmp = nextInt(buffer, offsetVal);
			while(tmp != 0) {
				last += tmp;
				long long lastX = last >> logTileSize; // transpose
				long long lastY = last - (lastX << logTileSize);
				long long tmpLast = (lastY << logTileSize) + lastX;
				if((tmpLast >= interestedFrom) && (tmpLast < interestedTo))
					sucArr[(*len)++] = offX + (int)(tmpLast - interestedFrom);
				tmp = nextInt(buffer, offsetVal);
			}
		} else { // zipped
			int tmpLen = maxTileElements;
			inflateBytes(buffer+offsetVal, length, tmpB, tmpLen);
			unsigned int tmpStart = 0;
			last = -1;
			tmp = nextInt(tmpB, tmpStart);
			while(tmp != 0) {
				last += tmp;
				long long lastX = last >> logTileSize; // transpose
				long long lastY = last - (lastX << logTileSize);
				long long tmpLast = (lastY << logTileSize) + lastX;
				if((tmpLast >= interestedFrom) && (tmpLast < interestedTo))
					sucArr[(*len)++] = offX + (int)(tmpLast - interestedFrom);
				tmp = nextInt(tmpB, tmpStart);
			}
		}
	}
}

long long CCompressedTile::nextInt(unsigned char *buf, unsigned int &start) {
	long long ret = buf[start];
	int len = ret & 3;
	switch(len) {
	case 0:
		start++;
		break;
	case 1:
		memcpy(&ret, buf+start, 2);
		start += 2;
		break;
	case 2:
		memcpy(&ret, buf+start, 4);
		start += 4;
		break;
	case 3:
		memcpy(&ret, buf+start, 8);
		start += 8;
		break;
	}
	return ret >> 2;
}

void CCompressedTile::inflateBytes(unsigned char *inBuffer, int ibLen, unsigned char *outBuffer, int &obLen) {
		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = (unsigned int)ibLen;
		strm.next_in = inBuffer;
		int ret = inflateInit2(&strm, -15);
/*		if(ret != Z_OK)
			return false;
*/		strm.avail_out = (unsigned int)obLen;
		strm.next_out = outBuffer;
		ret = inflate(&strm, Z_FINISH);
/*		if((ret == Z_STREAM_ERROR) || (ret == Z_NEED_DICT) || (ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR))
				return false;
*/		obLen -= (int)strm.avail_out;
		inflateEnd(&strm);
}