#include "CChunk.h"
#include "definitions.h"
#include <zlib.h>

CChunk::CChunk(int maxNodeNr, const int h) {
	type = 1;
	prepare(maxNodeNr, h);
}

CChunk::CChunk(unsigned char *buffer, int length, int maxNodeNr, const int h, int maxUncomprSize) {

	type = 2;

	calculateBits(maxNodeNr, h);

	uncomprBuffer = new unsigned char [maxUncomprSize];
	
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = (unsigned int)length;
	strm.next_in = buffer;
	int ret = inflateInit2(&strm, -15);
	strm.avail_out = (unsigned int)maxUncomprSize;
	strm.next_out = uncomprBuffer;
	ret = inflate(&strm, Z_FINISH);
	inflateEnd(&strm);

	this->h = h;
}

int CChunk::byteEncode(int val, unsigned char *buffer) {
	if(val < 64) {
		buffer[0] = val << 2;
		return 1;
	}
	if(val < 16384) {
		buffer[0] = (val << 2) + 1;
		buffer[1] = val >> 6;
		return 2;
	}
	buffer[0] = (val << 2) + 2;
	for(int i=1; i<b; i++)
		buffer[i] = (val >> (-2 + (i << 3))) & 255;
	return b;
}

int CChunk::byteEncode1bit(int val, unsigned char *buffer) {
	if(val < 128) {
		buffer[0] = val;
		return 1;
	}
	if(val < 16384) {
		buffer[0] = 128 | (val & 63);
		buffer[1] = val >> 6;
		return 2;
	}
	buffer[0] = 192 | (val & 63);
	for(int i=1; i<b; i++)
		buffer[i] = (val >> (-2 + (i << 3))) & 255;
	return b;
}

CChunk::~CChunk() {
	if(type == 1) {
		for(int i=0; i<h; i++)
			delete[] flagsBitArrays[i];
		delete[] flagsBitArrays;
		delete[] residArray;
	} else {
		delete[] uncomprBuffer;
	}
}

void CChunk::calculateBits(int maxNodeNr, const int h) {
	bitsPerH = 0;
	for(this->h = 1; this->h != h; this->h <<= 1)
		bitsPerH++;

	bytesPerH = h >> 3;

	b = (maxNodeNr < 4194304) ? 3 : 4;
}

void CChunk::prepare(int maxNodeNr, const int h) {
	flagsBitArrays = new unsigned char*[h];
	for(int i=0; i<h; i++) {
		flagsBitArrays[i] = new unsigned char[ (maxNodeNr + 7) >> 3 ];
		memset(flagsBitArrays[i], 0, (maxNodeNr + 7) >> 3);
	}
	residArray = new int[ maxNodeNr << 2 ];

	calculateBits(maxNodeNr, h);
	
	numResid = 0;
}

void CChunk::createEdge(int x, int y) {
	residArray[numResid++] = x;
	flagsBitArrays[y][ x >> 3 ]  |=  1 << (x & 7);
}

int CChunk::compressBytes(unsigned char *inputB, int length, unsigned char *outBuffer, int bufferBytesLeft) {
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	int ret = deflateInit2(&strm, 9, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
	strm.avail_in = (unsigned int)length;
	strm.next_in = inputB;
	strm.avail_out = (unsigned int)(bufferBytesLeft);
	strm.next_out = outBuffer;
	ret = deflate(&strm, Z_FINISH); // we make it only once, so Z_FLUSH
	deflateEnd(&strm);
	return bufferBytesLeft - strm.avail_out;
}

int compare (const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}

int CChunk::process(unsigned char *outBuffer, int bufferBytesLeft, int *maxResidListSize, int *maxUncomprSize) {

	qsort(residArray, numResid, sizeof(int), compare);
	unsigned char *uncomprChunkBuffer = new unsigned char[ chunkUncomprBufferSize ];
	int uncomprChunkBufferPos = 0;

	int numUniqueResid = 0, last = -1;
	for(int i=0; i<numResid; i++)
		if(residArray[i] != last) {
			uncomprChunkBufferPos += byteEncode(residArray[i] - last, uncomprChunkBuffer + uncomprChunkBufferPos);
			last = residArray[numUniqueResid++] = residArray[i];
		}
	uncomprChunkBufferPos += byteEncode(0, uncomprChunkBuffer + uncomprChunkBufferPos);

	for(int i=0; i<numUniqueResid; i++, uncomprChunkBufferPos += bytesPerH)
		for(int j=0; j<h; j++) {
			int succNr = residArray[i];
			if(flagsBitArrays[j][succNr >> 3] & (1 << (succNr & 7))) {
				flagsBitArrays[j][succNr >> 3] &= ~(1 << (succNr & 7));
				uncomprChunkBuffer[uncomprChunkBufferPos + (j >> 3)] |= 1 << (j >> 3);
			}
		}

	numResid = 0;

	int ret = compressBytes(uncomprChunkBuffer, uncomprChunkBufferPos, outBuffer, bufferBytesLeft);

	delete[] uncomprChunkBuffer;

	if(numUniqueResid > *maxResidListSize)
		*maxResidListSize = numUniqueResid;

	if(uncomprChunkBufferPos > *maxUncomprSize)
		*maxUncomprSize = uncomprChunkBufferPos;

	return ret;

}

void CChunk::getSucc(const int row, int **sucArr, int *len, int maxResidListSize) {
	
	int *resids = new int [maxResidListSize];
	int numResids = 0;

	int succNodeNr = -1, pos = 0, val;
	val = uncomprBuffer[pos++];
	while(val != 0) {
		switch(val & 3) {
		case 0:
			val >>= 2;
			break;
		case 1:
			val >>= 2;
			val += uncomprBuffer[pos++] << 6;
			break;
		case 2:
			val >>= 2;
			for(int i=1; i<b; i++)
				val += uncomprBuffer[pos++] << ((i << 3) - 2);
		}
		succNodeNr += val;
		resids[numResids++] = succNodeNr;
		val = uncomprBuffer[pos++];
	}

	*len = 0;

	for(int i=0; i<numResids; i++)
		if(uncomprBuffer[pos + (row >> 3)] & (1 << (row & 7)))
			resids[(*len)++] = resids[i];

	(*sucArr) = new int [ *len ];
	memcpy(*sucArr, resids, *len);
	
	delete[] resids;
}