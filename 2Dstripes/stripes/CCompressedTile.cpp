#include "CCompressedTile.h"
#include <cstdio>
#include <zlib.h>
#include <string>

CCompressedTile::CCompressedTile() {
	tmpB = buffer = 0;
}

CCompressedTile::~CCompressedTile() {
	delete[] tmpB;
}

void CCompressedTile::setParams(int tile_size, int strip_size, int log_tile_size, unsigned char *buffer, int *outArr) {
	tileSize = tile_size;
	stripSize = strip_size;
	logTileSize = log_tile_size;
	bytesPerStrip = (stripSize+7) >> 3;
	lengthOffset = 6 + (bytesPerStrip<<1);
	this->buffer = buffer;
	this->outArr = outArr;
	delete[] tmpB;
	tmpB = new unsigned char[tileSize << logTileSize];
	defaultTmpLen = tileSize << logTileSize;
}

void CCompressedTile::setY(int yInTile) {
	y = yInTile;
	interestedFrom = y << logTileSize;
	interestedTo = interestedFrom + tileSize;
	tmpY = (y * stripSize) >> logTileSize;
	yStripOffset = lengthOffset - bytesPerStrip - (tmpY>>3);
	tmpYmask = 1 << (tmpY & 7);
	len = 0;
}

void CCompressedTile::setX(int xInTile) {
	x = xInTile;
	interestedFrom = x << logTileSize;
	interestedTo = interestedFrom + tileSize;
	tmpX = (x * stripSize) >> logTileSize; // xStripe nr tmpX is to be checked for 0/1
	xStripOffset = lengthOffset - (tmpX>>3);
	tmpXmask = 1 << (tmpX & 7);
	len = 0;
}

void CCompressedTile::decompressX(int offset, int nextOffset) {
	offsetVal = offset & 0x3FFFFFFF;
	nextOffsetVal = nextOffset & 0x3FFFFFFF;
	length = nextOffsetVal - offsetVal;
	x = (buffer[nextOffsetVal-6] + ((int)buffer[nextOffsetVal-5] << 8) + ((int)buffer[nextOffsetVal-4] << 16)) << logTileSize; // copy x offset and multiply by tilesize
	if((buffer[nextOffsetVal-yStripOffset] & tmpYmask) != 0) { // if there is something
		if((offset & 0x80000000) == 0) { // normal, not transposed
			if((offset & 0x40000000) == 0) { // no zip
				last = -1;
				tmp = nextInt(buffer, offsetVal);
				while(tmp != 0) {
					last += tmp;
					if(last >= interestedFrom)
						if(last < interestedTo)
							outArr[len++] = x + last - interestedFrom;
						else
							break;
					tmp = nextInt(buffer, offsetVal);
				}
			} else { // zipped
				tmpLen = defaultTmpLen;
				inflateBytes(buffer+offsetVal, length-lengthOffset, tmpB, tmpLen);
				tmpStart = 0;
				last = -1;
				tmp = nextInt(tmpB, tmpStart);
				while(tmp != 0) {
					last += tmp;
					if(last >= interestedFrom)
						if(last < interestedTo)
							outArr[len++] = x + last - interestedFrom;
						else
							break;
					tmp = nextInt(tmpB, tmpStart);
				}
			}
		} else { // transposed
			if((offset & 0x40000000) == 0) { // no zip
				last = -1;
				tmp = nextInt(buffer, offsetVal);
				while(tmp != 0) {
					last += tmp;
					lastX = last >> logTileSize; // transpose
					lastY = last - (lastX << logTileSize);
					if(lastY == y)
						outArr[len++] = x + lastX;
					tmp = nextInt(buffer, offsetVal);
				}
			} else { // zipped
				tmpLen = defaultTmpLen;
				inflateBytes(buffer+offsetVal, length-lengthOffset, tmpB, tmpLen);
				tmpStart = 0;
				last = -1;
				tmp = nextInt(tmpB, tmpStart);
				while(tmp != 0) {
					last += tmp;
					lastX = last >> logTileSize; // transpose
					lastY = last - (lastX << logTileSize);
					if(lastY == y)
						outArr[len++] = x + lastX;
					tmp = nextInt(tmpB, tmpStart);
				}
			}
		}
	}
}

void CCompressedTile::decompressY(int offset, int nextOffset) {
	offsetVal = offset & 0x3FFFFFFF;
	nextOffsetVal = nextOffset & 0x3FFFFFFF;
	length = nextOffsetVal - offsetVal;
	y = (buffer[nextOffsetVal-3] + ((int)buffer[nextOffsetVal-2] << 8) + ((int)buffer[nextOffsetVal-1] << 16)) << logTileSize; // copy y offset and multiply by tilesize
	if((buffer[nextOffsetVal-xStripOffset] & tmpXmask) != 0) { // if there is something
		if((offset & 0x80000000) == 0) { // normal, not transposed
			if((offset & 0x40000000) == 0) { // no zip
				last = -1;
				tmp = nextInt(buffer, offsetVal);
				while(tmp != 0) {
					last += tmp;
					lastY = last >> logTileSize; // transpose
					lastX = last - (lastY << logTileSize);
					if(lastX == x)
						outArr[len++] = y + lastY;
					tmp = nextInt(buffer, offsetVal);
				}
			} else { // zipped
				tmpLen = defaultTmpLen;
				inflateBytes(buffer+offsetVal, length-lengthOffset, tmpB, tmpLen);
				tmpStart = 0;
				last = -1;
				tmp = nextInt(tmpB, tmpStart);
				while(tmp != 0) {
					last += tmp;
					lastY = last >> logTileSize; // transpose
					lastX = last - (lastY << logTileSize);
					if(lastX == x)
						outArr[len++] = y + lastY;
					tmp = nextInt(tmpB, tmpStart);
				}
			}
		} else { // transposed
			if((offset & 0x40000000) == 0) { // no zip
				last = -1;
				tmp = nextInt(buffer, offsetVal);
				while(tmp != 0) {
					last += tmp;
					if(last >= interestedFrom)
						if(last < interestedTo)
							outArr[len++] = y + last - interestedFrom;
						else
							break;
					tmp = nextInt(buffer, offsetVal);
				}
			} else { // zipped
				tmpLen = defaultTmpLen;
				inflateBytes(buffer+offsetVal, length-lengthOffset, tmpB, tmpLen);
				tmpStart = 0;
				last = -1;
				tmp = nextInt(tmpB, tmpStart);
				while(tmp != 0) {
					last += tmp;
					if(last >= interestedFrom)
						if(last < interestedTo)
							outArr[len++] = y + last - interestedFrom;
						else
							break;
					tmp = nextInt(tmpB, tmpStart);
				}
			}
		}
	}
}

int CCompressedTile::nextInt(unsigned char *buf, int &start) {
	int ret = buf[start];
	int mask = ret & 3;
	switch(mask) {
	case 0:
		start++;
		return ret >> 2;
	case 1:
		start += 2;
		return ((int)buf[start-1] << 6) + (ret >> 2);
	default:
		start += 3;
		return ((int)buf[start-1] << 14) + ((int)buf[start-2] << 6) + (ret >> 2);
	}
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

int CCompressedTile::getLen() {
	return len;
}