#include "BFBuffer.h"
#include "definitions.h"

BFBuffer::BFBuffer(long long init, int bDiff) {
	buffer = new unsigned char[init];
	size = init;
	start = 0;
	b = bDiff;
}

BFBuffer::~BFBuffer() {
	delete[] buffer;
}

void BFBuffer::byteEncode(long long val) {
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

unsigned char *BFBuffer::getFlagBuffer() { // return pointer to the place of the buffer, where we can store flags
	memset(buffer+start, 0, size-start);
	return buffer + start;
}

unsigned char *BFBuffer::getBuffer() {
	return buffer;
}

long long BFBuffer::getListLength() {
	return start;
}