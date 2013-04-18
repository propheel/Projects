#include "BFBuffer.h"
#include "definitions.h"

BFBuffer::BFBuffer(int bDiff) {
	buffer = new unsigned char[residBuffer];
	size = residBuffer;
	memset(buffer, 0, residBuffer);
	start = 0;
	b = bDiff;
	offsets = new long long [chunksPerZip];
	offCnt = 0;
}

BFBuffer::~BFBuffer() {
	delete[] buffer;
	delete[] offsets;
}

void BFBuffer::clear() {
	start = 0;
	memset(buffer, 0, size);
	offCnt = 0;
}

void BFBuffer::addOffset(long long offset) {
	start += offset;
	offsets[offCnt++] = start;
}

void BFBuffer::byteEncode(long long val) {
	if((start + b) > size)
		resize();
	if(val < 64) 
		buffer[start++] = (unsigned char)(val<<2);
	else if(val < 16384) {
		buffer[start++] = (unsigned char)(((val<<2)+1) & 255); // 0000000011111111
		buffer[start++] = (unsigned char)((val>>6) & 255);     // 1111111100000000
	} else {
		buffer[start++] = (unsigned char)(((val<<2)+3) & 255);
		for(int i=1; i<b; i++)
			buffer[start++] = (unsigned char)((val>>((i<<3)-2)) & 255);
	}
}

void BFBuffer::resize() {
	size <<= 3;
	unsigned char *newB = new unsigned char[size];
	memset(newB+start, 0, size - start);
	memcpy(newB, buffer, start);
	delete[] buffer;
	buffer = newB;
}

unsigned char *BFBuffer::getFlagBuffer(long long space) { // return pointer to the place of the buffer, where we can store flags
	if((start + space) > size)
		resize();
	return buffer + start;
}

unsigned char *BFBuffer::getBuffer() {
	return buffer;
}

long long BFBuffer::getListLength() {
	return start;
}

void BFBuffer::addOffsetsAtEnd() {
	if((start + ((chunksPerZip-1)<<2)) > size)
		resize();
	for(int i=0; i<chunksPerZip-1; i++)
		for(int j=3; j>-1; j--)
			buffer[start++] = (offsets[i] >> (j << 3)) & 255;
}