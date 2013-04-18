#include "WG4Reader.h"

WG4Reader::WG4Reader() {
	errorNr = 1;
}

WG4Reader::WG4Reader(const char *inputFileName) {
	open(inputFileName);
}

void WG4Reader::open(const char *inputFileName) {
	f = fopen(inputFileName, "rb");
	if(f == NULL) { // check if the file exists
		errorNr = 2;
		return;
	}

	_fseeki64(f, 0, SEEK_END); // check file size
	long long fSize = _ftelli64(f);
	rewind(f);

	if(fread(&nodes, 4, 1, f) < 1) { // read number of nodes
		errorNr = 2;
		fclose(f);
		return;
	}
	if(fread(&edges, 8, 1, f) < 1) { // read number of edges
		errorNr = 2;
		fclose(f);
		return;
	}
	if(fSize-12 != ((edges + nodes) << 2)) { // check whether the file is complete
		errorNr = 2;
		fclose(f);
		return;
	}

	// if everything is ok, go to the first row and create the buffer
	buffer = new unsigned char[readChunk<<1]; // double the size in order to be able to move some not processed data
	if(buffer == NULL) {
		errorNr = 3;
		fclose(f);
		return;
	}
	bufferSize = (int)fread(buffer, 1, readChunk, f);
	bufferPos = 0;
	errorNr = 0;
}

WG4Reader::~WG4Reader() {
	if(errorNr == 0) {
		delete[] buffer;
		fclose(f);
	}
}

bool WG4Reader::error() {
	return (errorNr != 0);
}

int WG4Reader::numNodes() {
	return nodes;
}

long long WG4Reader::numEdges() {
	return edges;
}

void WG4Reader::getNumSucc(int *succNr) {
	checkIfThereAreBytesInBuffer(4);
	takeFromBuffer(succNr, 4);
	checkIfThereAreBytesInBuffer((*succNr) << 2);
}

void WG4Reader::getNextSucc(int *succNodeNr) {
	takeFromBuffer(succNodeNr, 4);
}

void WG4Reader::checkIfThereAreBytesInBuffer(int howManyBytes) {
	if(bufferPos + howManyBytes > bufferSize) {
		bufferSize -= bufferPos;
		memmove(buffer, buffer + bufferPos, bufferSize);
		bufferPos = 0;
		bufferSize += fread(buffer + bufferSize, 1, readChunk, f);
	}
}

void WG4Reader::takeFromBuffer(void *destBuff, int howManyBytes) {
	memcpy(destBuff, buffer + bufferPos, howManyBytes);
	bufferPos += howManyBytes;
}