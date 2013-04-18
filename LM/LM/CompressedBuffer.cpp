/*#include "CompressedBuffer.h"
#include "definitions.h"
#include <zlib.h>

CompressedBuffer::CompressedBuffer() {
	buffer = NULL;
	size = offset = 0;
}

CompressedBuffer::~CompressedBuffer() {
	delete[] buffer;
}

void CompressedBuffer::reset(long long initSize) {
	delete[] buffer;
	buffer = new unsigned char[initSize];
	if(buffer == NULL) {
		printf("Memory allocation error (%lld wanted).\n", initSize);
		size = 0;
		return;
	}
	size = initSize;
	offset = 0;
}

void CompressedBuffer::compressBytes(unsigned char *inputB, long long length) {
		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		int ret = deflateInit2(&strm, 9, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
		strm.avail_in = (unsigned int)length;
		strm.next_in = inputB;
		strm.avail_out = (unsigned int)(size - offset);
		strm.next_out = buffer + offset;
		ret = deflate(&strm, Z_FINISH); // we make it only once, so Z_FLUSH
		offset += (size - offset) - strm.avail_out;
		deflateEnd(&strm);
}

long long CompressedBuffer::getOffset() {
	return offset;
}

void CompressedBuffer::setOffset(long long off) {
	offset = off;
}

unsigned char * CompressedBuffer::getBuffer() {
	return buffer;
}

bool CompressedBuffer::uncompressBytes(unsigned char *outputB, long long &outSize, long long offset, long long len) {
		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = (unsigned int)len;
		strm.next_in = &buffer[offset];
		int ret = inflateInit2(&strm, -15);
		strm.avail_out = (unsigned int)outSize;
		strm.next_out = outputB;
		ret = inflate(&strm, Z_FINISH);
		outSize -= (long long)strm.avail_out;
		inflateEnd(&strm);
		return (ret == Z_STREAM_END);
}*/