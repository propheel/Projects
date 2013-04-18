#include "LMGraph.h"
#include "definitions.h"
#include "WG4Reader.h"
#include "CChunk.h"

LMGraph::LMGraph() {
	loaded = false;
}

LMGraph::~LMGraph() {
	if(loaded)
		unLoad();
}

void LMGraph::resizeBuffer() {
	int newS = graphSize << 1;
	unsigned char *newB = new unsigned char [ newS ];
	if(newB == NULL) {
		printf("Couldn't allocate %d bytes.\n", newS);
		return;
	}
	memcpy(newB, graphBuffer, graphSize);
	delete[] graphBuffer;
	graphBuffer = newB;
	graphSize = newS;
}

void LMGraph::loadFromTextFile(const char *inputFileName, const int h) { // load graph from wg4 file

	unLoad();

	// open input file
	WG4Reader reader(inputFileName);
	if(reader.error())
		return;

	numNodes = reader.numNodes();
	numEdges = reader.numEdges();

	maxSuccListLength = maxResidListLength = maxUncomprSize = 0;

	offsets = new unsigned int[ (numNodes + (h << 1) - 1) / h ];
	offsetsCnt = 0;

	graphBuffer = new unsigned char[ initGraphBufferSize ];
	graphPos = 0;
	graphSize = initGraphBufferSize;

	CChunk chunk(numNodes, h);

	// read all nodes
	for(int nodeNr = 0, hCounter = 0; nodeNr < numNodes; nodeNr++) {

		if((nodeNr >> 16) << 16 == nodeNr)
			printf(".");

		int numSucc;
		reader.getNumSucc(&numSucc);

		// check for maximum list length
		if(numSucc > maxSuccListLength)
			maxSuccListLength = numSucc;

		// get all successors
		for(int succNr = 0; succNr < numSucc; succNr++) {

			// take successor node number
			int succNodeNr;
			reader.getNextSucc(&succNodeNr);

			// create an edge
			//chunk.createEdge(nodeNr, succNodeNr);
			chunk.createEdge(succNodeNr, hCounter);

		}

		hCounter++;

		// process each chunk
		if((hCounter == h) || (nodeNr == numNodes-1)) {

			// add offset
			offsets[offsetsCnt++] = graphPos;

			int oldPos = graphPos;

			graphPos += chunk.process(graphBuffer + graphPos, graphSize - graphPos, &maxResidListLength, &maxUncomprSize);

			// if buffer 90% filled, resize it
			if((unsigned int)(10 * graphPos)  >  (9 * graphSize))
				resizeBuffer();

			hCounter = 0;

			// if it is the last chunk, add additional offset
			if(nodeNr == numNodes-1)
				offsets[offsetsCnt++] = graphPos;

		}
	}

	this->h = h;

	loaded = true;
}

void LMGraph::loadFromFiles(const char *graphFileName, const char *offsetsFileName, const int h) {

	unLoad();

	// read graph file
	FILE *f = fopen(graphFileName, "rb");
	if(f == NULL) {
		printf("Couldn't open %s for reading.\n", graphFileName);
		return;
	}
	_fseeki64(f, 0, SEEK_END);
	graphPos = graphSize = (unsigned int)_ftelli64(f);
	rewind(f);
	graphBuffer = new unsigned char [ graphSize ];
	if(graphBuffer == NULL) {
		fclose(f);
		return;
	}
	fread(graphBuffer, 1, graphSize, f);
	fclose(f);

	// read offsets file
	f = fopen(offsetsFileName, "rb");
	if(f == NULL) {
		printf("Couldn't open %s for reading.\n", offsetsFileName);
		return;
	}
	_fseeki64(f, 0, SEEK_END);
	int fSize = (int)_ftelli64(f);
	rewind(f);
	offsetsCnt = (fSize - 16) >> 2;
	if(offsetsCnt < 2) {
		delete[] graphBuffer;
		fclose(f);
		return;
	}
	offsets = new unsigned int [offsetsCnt];
	fread(&numNodes, 4, 1, f);
	fread(&maxSuccListLength, 4, 1, f);
	fread(&maxResidListLength, 4, 1, f);
	fread(&maxUncomprSize, 4, 1, f);
	fread(offsets, 4, offsetsCnt, f);

	this->h = h;

	loaded = true;
}

void LMGraph::successorArray(const int row, int **sucArr, int *len) {
	int offsetNr = row / h;

	CChunk chunk(graphBuffer + offsets[offsetNr], offsets[offsetNr+1] - offsets[offsetNr], numNodes, h, maxUncomprSize);

	chunk.getSucc(row - (offsetNr*h), sucArr, len, maxResidListLength);
}

unsigned int LMGraph::getSize() {
	return graphPos + (offsetsCnt << 2) + (4 << 2);
}

void LMGraph::unLoad() {
	if(loaded) {
		delete[] graphBuffer;
		delete[] offsets;
		loaded = false;
	}
}

void LMGraph::saveToFiles(const char *graphFileName, const char *offsetsFileName) {

	// write graph file
	FILE *f = fopen(graphFileName, "wb");
	if(f == NULL) {
		printf("Couldn't open %s for writing.\n", graphFileName);
		return;
	}
	fwrite(graphBuffer, 1, graphPos, f);
	fclose(f);

	// write offsets file
	f = fopen(offsetsFileName, "wb");
	if(f == NULL) {
		printf("Couldn't open %s for writing.\n", offsetsFileName);
		return;
	}
	fwrite(&numNodes, 4, 1, f);
	fwrite(&maxSuccListLength, 4, 1, f);
	fwrite(&maxResidListLength, 4, 1, f);
	fwrite(&maxUncomprSize, 4, 1, f);
	fwrite(offsets, 4, offsetsCnt, f);
}

/* 
#include "LMGraph.h"
#include "BFBuffer.h"
#include "definitions.h"

LMGraph::LMGraph() {
	bData = NULL;
	offsets = outArr = NULL;
}

LMGraph::~LMGraph() {
	delete[] outArr;
	delete[] bData;
	delete[] offsets;
}




void LMGraph::calculateBits() {
	bytesPerH = (h+7)/8; // used in coding flags
	bitsPerH = 0;
	for(int i=1; i<h; i<<=1)
		bitsPerH++;
	b = 1; // used for byte coding (1, 2, b)
	for(long long i=64; i<lineNum; i<<=8)
		b++;
}




int compare (const void * a, const void * b) {
	return (int)( *(long long*)a - *(long long*)b );
}




// read the graph from the text file (each line is a node and contains integers pointing to another node)
bool LMGraph::loadFromTextFile(const char *fileName, const int h) {

	// delete recent data
	delete[] outArr;
	delete[] bData;
	delete[] offsets;

	// try to open file
	FILE *f = fopen(fileName, "rb");

	long long fileSize;
	// check file size
	_fseeki64(f, 0, SEEK_END);
	fileSize = _ftelli64(f);
	rewind(f);

	long long fileLeft = fileSize, tmp = 0;
	unsigned char *readBuffer = new unsigned char[readChunk << 1]; // we need bigger chunk because there can be some bytes left before reading next chunk
	long long readBufferStart = 0, readBufferEnd = 0, bytesRead;
	lineNum = 0;

	// check lineNum
	while(fileLeft > 0) {
		if(fileLeft > readChunk)
			bytesRead = fread(readBuffer, 1, readChunk, f);
		else
			bytesRead= fread(readBuffer, 1, fileLeft, f);
		fileLeft -= bytesRead;
		for(int i=0; i<bytesRead; i++)
			if(readBuffer[i] == '\n')
				lineNum++;
	}
	rewind(f);
	fileLeft = fileSize;

	// set parameters
	this->h = h;
	calculateBits();

	// prepare for the processing
	long long i = 0;
	offsetsAmount = (lineNum + h - 1) / h; // number of chunks
	offsetsAmount = (offsetsAmount + (chunksPerZip<<2) - 1) / chunksPerZip; // number of zips
	offsets = new long long[offsetsAmount];
	offsetsAmount = maxListLength = maxResidListLength = maxUncomprSize = 0;
	int count = 0;
	compressedGraph.reset(fileSize>>2); // create buffer for output
	
	// create output buffer (to be compressed)
	BFBuffer outRawBuffer(b);
	int chunksNotZipped = 0;
	
	char **sets = new char*[h];
	for(int i=0; i<h; i++) {
		sets[i] = new char[lineNum>>3];
		memset(sets[i], 0, (lineNum>>3));
	}
	long long *allInts = new long long[lineNum<<1];
	long long listSize, allIntsSize = 0;

	while(fileLeft > 0) {
		if(fileLeft > readChunk)
			bytesRead = fread(readBuffer + readBufferStart, 1, readChunk, f);
		else {
			bytesRead= fread(readBuffer + readBufferStart, 1, fileLeft, f);
			fclose(f);
		}
		fileLeft -= bytesRead;
		readBufferEnd = readBufferStart + bytesRead;
		readBufferStart = 0;

		// find last newline
		long long lastNewLine = readBufferEnd-1;
		while(readBuffer[lastNewLine] != '\n') // we assume, that for given chunk there will be at least one new line
			lastNewLine--;
		lastNewLine++;

		while(readBufferStart < lastNewLine) {
			listSize = allIntsSize;
			while(readBuffer[readBufferStart] != '\n') {
				if(readBuffer[readBufferStart] == ' ') { // add number to the set
					sets[count][tmp>>3] |= 1 << (tmp & 7);
					allInts[allIntsSize++] = tmp;
					tmp = 0;
				} else tmp = (tmp<<3) + (tmp<<1) + readBuffer[readBufferStart] - '0';
				readBufferStart++;
			}
			readBufferStart++;

			// check for max length
			listSize = allIntsSize - listSize;
			if(listSize > maxListLength)
				maxListLength = listSize;
			
			count++;

			if(i == ((i >> 17) << 17))
				printf(".");
			
			// if enough lines, create a chunk
			if((count == h) || (i == lineNum-1)) {

				// save current offset
				if(chunksNotZipped==0)
					offsets[offsetsAmount++] = compressedGraph.getOffset();

				chunksNotZipped++;
	
				qsort(allInts, allIntsSize, sizeof(long long), compare);
				long long allIntsElements = 0, last = -1;
				for(long long m=0; m<allIntsSize; m++)
					if(allInts[m] != last)
						allInts[allIntsElements++] = last = allInts[m];

				// check for max residues size
				if(allIntsElements > maxResidListLength)
					maxResidListLength = allIntsElements;

				// outB <- byteEncode(diffEncode(allInts))
				long long lastInt = -1;
				for(long long m=0; m<allIntsElements; m++) {
					outRawBuffer.byteEncode(allInts[m]-lastInt);
					lastInt = allInts[m];
				}
				outRawBuffer.byteEncode(0);
	
				// set flags for all residues and buckets
				long long l=0;
				unsigned char *flagBuffer = outRawBuffer.getFlagBuffer(allIntsElements*bytesPerH);
				for(long long m=0; m<allIntsElements; m++, l+=bytesPerH) {
					for(int j=0; j<h; j++) {
						long long tmpA = allInts[m];
						if(sets[j][tmpA>>3] & (1 << (tmpA & 7))) {
							sets[j][tmpA>>3] &= ~(1 << (tmpA & 7));
							flagBuffer[l + (j>>3)] |= (1<<(j & 7));
						}
					}
					allInts[m] = 0;
				}
				outRawBuffer.addOffset(l);
	
				// compress(concatenate(outB, outF))
				// outRawBuffer is already filled with concatenate(outB, outF), so just compress it
				if((chunksNotZipped == chunksPerZip) || (i == lineNum-1)) {

					outRawBuffer.addOffsetsAtEnd();

					// check max uncompressed size
					long long uncomprSize = outRawBuffer.getListLength();
					if(uncomprSize > maxUncomprSize)
						maxUncomprSize = uncomprSize;
	
					compressedGraph.compressBytes(outRawBuffer.getBuffer(), uncomprSize);
					outRawBuffer.clear();
					chunksNotZipped = 0;
	
					// if it is the last chunk, write offset
					if(i == lineNum-1)
						offsets[offsetsAmount++] = compressedGraph.getOffset();
				}

				// reset all the remaining variables
				count = 0;
				allIntsSize = 0;
			}
			i++;
		}
		memmove(readBuffer, readBuffer + readBufferStart, readBufferEnd - readBufferStart);
		readBufferStart = readBufferEnd - readBufferStart;
	}

	// reserve for fast successors
	bData = new unsigned char[maxUncomprSize];
	outArr = new long long[getMaxResidues()];

	// release resources
	for(int i=0; i<h; i++)
		delete[] sets[i];
	delete[] sets;
	delete[] allInts;
	delete[] readBuffer;
	return true;
}





// save graph and offsets to different files
bool LMGraph::saveToFile(const char *graphFileName, const char *offsetFileName) {

	// try to open files
	FILE *g = fopen(graphFileName, "wb");
	FILE *o = fopen(offsetFileName, "wb");
	if(g == NULL) {
		printf("Error: cannot open %s for writing.\n", graphFileName);
		return false;
	}
	if(o == NULL) {
		printf("Error: cannot open %s for writing.\n", offsetFileName);
		fclose(g);
		return false;
	}

	// save graph and close file
	fwrite(compressedGraph.getBuffer(), sizeof(unsigned char), compressedGraph.getOffset(), g);
	fclose(g);

	// save offsets
	for(long long i=0; i<offsetsAmount; i++)
		fwrite(offsets+i, 4, 1, o);

	// save additional parameters and close file
	fwrite(&lineNum, 4, 1, o);
	fwrite(&maxUncomprSize, 4, 1, o);
	fwrite(&maxResidListLength, 4, 1, o);
	fwrite(&maxListLength, 4, 1, o);
	fclose(o);

	return true;
}




// load graph and offsets from their files
bool LMGraph::loadFromFile(const char *graphFileName, const char *offsetFileName, const int h) {

	// delete recent data
	delete[] outArr;
	delete[] bData;
	delete[] offsets;

	// try to open files
	FILE *g = fopen(graphFileName, "rb");
	FILE *o = fopen(offsetFileName, "rb");
	if(g == NULL) {
		printf("Error: cannot open %s for reading.\n", graphFileName);
		return false;
	}
	if(o == NULL) {
		printf("Error: cannot open %s for reading.\n", offsetFileName);
		fclose(g);
		return false;
	}

	long long fileSize;
	// check graph file size
#ifdef OS_WIN
	_fseeki64(g, 0, SEEK_END);
	fileSize = _ftelli64(g);
#elif defined OS_LINUX
	fseeko(g, 0, SEEK_END);
	fileSize = ftello(g);
#endif
	rewind(g);

	// allocate memory for reading graph
	compressedGraph.reset(fileSize);

	// read and close graph file
	fread(compressedGraph.getBuffer(), sizeof(unsigned char), fileSize, g);
	compressedGraph.setOffset(fileSize);
	fclose(g);

	// check offsets file size
#ifdef OS_WIN
	_fseeki64(o, 0, SEEK_END);
	fileSize = _ftelli64(o);
#elif defined OS_LINUX
	fseeko(o, 0, SEEK_END);
	fileSize = ftello(o);
#endif
	rewind(o);

	offsetsAmount = (fileSize >> 2) - 4;
	// allocate memory for reading offsets
	offsets = new long long[offsetsAmount];

	// read offsets and close file
	for(int i=0; i<offsetsAmount; i++) {
		*(offsets+i) = 0;
		fread(offsets+i, 4, 1, o);
	}
	fread(&lineNum, 4, 1, o);
	fread(&maxUncomprSize, 4, 1, o);
	fread(&maxResidListLength, 4, 1, o);
	fread(&maxListLength, 4, 1, o);
	fclose(o);

	// calculate attributes
	this->h = h;
	calculateBits();
	bData = new unsigned char[maxUncomprSize];
	outArr = new long long[maxResidListLength];

	return true;
}




// save graph to the list of adjacency lists
bool LMGraph::saveToTextFile(const char *fileName) {

	// try to open file
	FILE *f = fopen(fileName, "wb");
	if(f == NULL) {
		printf("Error: cannot open %s for writing.\n", fileName);
		return false;
	}

	// for each node print its adjacency list and close file
	long long arrLen = 0;
	for(long long row = 0; row < lineNum; row++) {
		successorArray(row, arrLen);
		for(long long i=0; i<arrLen; i++)
			fprintf(f, "%lld ", outArr[i]);
		fprintf(f, "%c", 10);
	}
	delete[] outArr;
	fclose(f);

	return true;
}




// print some statistics
void LMGraph::printStats() {
	printf(" Nodes: %llu\n", lineNum);
	printf(" Graph memory size: %lld B\n", compressedGraph.getOffset());
	printf(" Offsets: %llu\n", offsetsAmount);
	printf(" Max length of list of residues: %llu\n", maxResidListLength);
	printf(" Max length of adjacency list: %llu\n", maxListLength);
	printf(" Max uncompressed chunk size: %llu\n", maxUncomprSize);
	printf(" Chunk size: %d\n", h);
	printf(" Bytes to store chunk: %d\n", bytesPerH);
	printf(" Bits to store chunk nr: %d\n", bitsPerH);
	printf(" Bytes to store max difference: %d\n", b);
	printf(" Total size: %lld\n", compressedGraph.getOffset()+((offsetsAmount+4)<<2));
}





// get array of successors of a row (require an array of length maxResidListLength and will NOT check if it is correct one)
long long * LMGraph::successorArray(long long row, long long &len) {

	// temp vars
	long long bDataLen = maxUncomprSize;

	// uncompress chunk
	long long chunkNr = (row >> bitsPerH) / chunksPerZip;
	compressedGraph.uncompressBytes(bData, bDataLen, offsets[chunkNr], offsets[chunkNr+1] - offsets[chunkNr]);

	// create a list of residues
	long long residListLen = 0, pos = 0;
	long long chunkInAPack = (row >> bitsPerH) - (chunkNr * chunksPerZip);
//printf("%lld %d\n", chunkNr, chunkInAPack);
	if(chunkInAPack != 0) {
		long long offPos = bDataLen + (( chunkInAPack - chunksPerZip ) << 2);
		for(int i=0; i<4; i++)
			pos = (pos << 8) + bData[offPos++];
	}
//printf("%d\n", pos);
	while(bData[pos] != 0) {
		long long val;
		switch(bData[pos] & 3) {
		case 1:
			val = (bData[pos]>>2) + (bData[pos+1]<<6);
			pos += 2;
			break;
		case 3:
			val = (bData[pos++]>>2);
			for(int i=1; i<b; i++)
				val += bData[pos++] << ((i<<3) - 2);
			break;
		default:
			val = bData[pos++]>>1;
		}
		outArr[residListLen++] = val;
	}
	pos++; // one char after 0
	len = 0;
	
	// if there are no successors leave
	if(residListLen==0)
		return NULL;

	// check for adjacency and store it
	outArr[0]--;
	long long relRow = row & (h-1),  bDataOffset = pos + (relRow >> 3),  mask = 1 << (relRow & 7);
	if((bData[bDataOffset] & mask) != 0)
		len = 1;
	for(long long i=1, ib=bytesPerH; i<residListLen; i++, ib+=bytesPerH) {
		outArr[i] += outArr[i-1];
		if((bData[ib + bDataOffset] & mask) != 0)
			outArr[len++] = outArr[i];
	}

	long long * sucArr = new long long[len];
	memcpy(sucArr, outArr, len*sizeof(long long));
	
	return sucArr;
}



long long LMGraph::getNodesAmount() {
	return lineNum;
}



long long LMGraph::getMaxResidues() {
	return maxResidListLength;
}



long long LMGraph::getLength() {
	return compressedGraph.getOffset()+((offsetsAmount+4)<<2);
}
*/