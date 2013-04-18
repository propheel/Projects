#include "CStripeGraph.h"
#include "CCompressedTile.h"
#include <cstdio>
#include <vector>
#include <set>

const int readChunk = 32768000;

CStripeGraph::CStripeGraph() {
	graphBuffer = 0;
	tileSize = tilesInARow = tilesAmount = 0;
	xOffsets = yOffsets = xFirstOffsets = yFirstOffsets = outArr = 0;
}

CStripeGraph::~CStripeGraph() {
	clear();
}

void CStripeGraph::clear() {
	delete[] graphBuffer;
	graphBuffer = 0;
	tileSize = tilesInARow = tilesAmount = 0;
	delete[] xOffsets;
	delete[] yOffsets;
	delete[] xFirstOffsets;
	delete[] yFirstOffsets;
	delete[] outArr;
	xOffsets = yOffsets = xFirstOffsets = yFirstOffsets = outArr = 0;
}

bool CStripeGraph::loadFromTextFile(const char *textFileName, const int tile_size, const int strip_size) {

	clear();

	// try to open file
	FILE *f = fopen(textFileName, "rb");

	long long fileSize;
	// check file size
	_fseeki64(f, 0, SEEK_END);
	fileSize = _ftelli64(f);
	rewind(f);

	long long fileLeft = fileSize, tmp = 0;
	unsigned char *readBuffer = new unsigned char[readChunk << 1];
	int readBufferStart = 0, readBufferEnd = 0, bytesRead;
	nodesAmount = 0;

	// check lineNum
	while(fileLeft > 0) {
		if(fileLeft > readChunk)
			bytesRead = fread(readBuffer, 1, readChunk, f);
		else
			bytesRead= fread(readBuffer, 1, fileLeft, f);
		fileLeft -= bytesRead;
		for(int i=0; i<bytesRead; i++)
			if(readBuffer[i] == '\n')
				nodesAmount++;
	}
	rewind(f);
	fileLeft = fileSize;

	// compute values (tileSize and stripSize must be a power of 2)
	logTileSize = 1;
	tileSize = 2;
	while(tileSize < tile_size) {
		logTileSize++;
		tileSize <<= 1;
	}
	stripSize = strip_size;
	tilesInARow = (nodesAmount + tileSize - 1) / tileSize;
	CUncompressedTile *tileRow = new CUncompressedTile[tilesInARow];
	for(int i=0; i<tilesInARow; i++)
		tileRow[i].setSizes(tileSize, logTileSize, stripSize);

	graphBuffer = new unsigned char[fileSize>>3]; // about fileSize / 8 should be enough
	graphBufferStart = 0;
	std::vector<int> xOffsetsVector;
	std::vector<int> *yOffsetsVectors = new std::vector<int>[tilesInARow];
	xFirstOffsets = new int[tilesInARow+1];
	yFirstOffsets = new int[tilesInARow+1];

//	printf(" tileSize: %d; stripSize: %d; tilesInARow: %d; memory reserved: %lld \n", tileSize, stripSize, tilesInARow, fileSize>>3);
	int compressed[4] = { 0, 0, 0, 0 };

	// process all lines
	int yInATile = 0, yTile = 0, i=0;

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
		int lastNewLine = readBufferEnd-1;
		while(readBuffer[lastNewLine] != '\n') // we assume, that for given chunk there will be at least one new line
			lastNewLine--;
		lastNewLine++;

		while(readBufferStart < lastNewLine) {

			// add all the edges to the corresponding tile
			long long tmp = 0;
			while(readBuffer[readBufferStart] != '\n') {
				if(readBuffer[readBufferStart] == ' ') {
					// add to tile
					long long xTileOffset = tmp >> logTileSize;
					tileRow[xTileOffset].addLink((int)(tmp - (xTileOffset << logTileSize)), yInATile);
					tmp = 0;
				} else tmp = (tmp<<3) + (tmp<<1) + readBuffer[readBufferStart] - '0';
				readBufferStart++;
			}
			readBufferStart++;
			yInATile++;

			if(i % 100000 == 0)
				printf(".");

			if((yInATile==tileSize) || (i==nodesAmount-1)) {

				xFirstOffsets[yTile] = -1;
			
				for(int j=0; j<tilesInARow; j++)
					if(tileRow[j].isNotEmpty()) {

						int offset = tileRow[j].compress(j, yTile, graphBuffer, graphBufferStart);
						tileRow[j].clear();

						compressed[(unsigned int)offset >> 30]++;

						if(yInATile != -1) {
							xFirstOffsets[yTile] = (int)xOffsetsVector.size();
							yInATile = -1;
						}

						yOffsetsVectors[j].push_back(xOffsetsVector.size());

						xOffsetsVector.push_back(offset);

					}
				
				yTile++;
				yInATile = 0;

			}
			i++;
		}
		memmove(readBuffer, readBuffer + readBufferStart, readBufferEnd - readBufferStart);
		readBufferStart = readBufferEnd - readBufferStart;
	}
	
	delete[] readBuffer;
	delete[] tileRow;

	tilesAmount = (int)xOffsetsVector.size();

	xFirstOffsets[tilesInARow] = yFirstOffsets[tilesInARow] = tilesAmount;

	yInATile = 0;
	for(int i=0; i<tilesInARow; i++) {
		yFirstOffsets[i] = yInATile;
		yInATile += yOffsetsVectors[i].size();
	}

	for(int i=tilesInARow-1; i>=0; i--) {
		if(xFirstOffsets[i] == -1)
			xFirstOffsets[i] = xFirstOffsets[i+1];

		if(yOffsetsVectors[i].size() == 0)
			yFirstOffsets[i] = yFirstOffsets[i+1];
	}

	xOffsets = new int[tilesAmount+1];
	for(int i=0; i<tilesAmount; i++)
		xOffsets[i] = xOffsetsVector[i];
	xOffsets[tilesAmount] = graphBufferStart;

	yOffsets = new int[tilesAmount+1];
	for(int x=0, i=0; x<tilesInARow; x++)
		for(auto it=yOffsetsVectors[x].begin(); it!=yOffsetsVectors[x].end(); it++)
			yOffsets[i++] = *it;
	yOffsets[tilesAmount] = graphBufferStart;
	
	delete[] yOffsetsVectors;
	delete[] outArr;

	outArr = new int[nodesAmount];
	tileForUncompress.setParams(tileSize, stripSize, logTileSize, graphBuffer, outArr);

	printf("%d B - size\n", size());
	printf("1:%d 2:%d 3:%d 4:%d\t", compressed[0], compressed[1], compressed[2], compressed[3]);

	return true;
}

bool CStripeGraph::loadFromFile(const char *graphFileName, const char *offsetsFileName, const int tile_size, const int strip_size) {

	clear();

	logTileSize = 1;
	tileSize = 2;
	while(tileSize < tile_size) {
		logTileSize++;
		tileSize <<= 1;
	}
	stripSize = strip_size;
	
	FILE *f = fopen(graphFileName, "rb");
	fseek(f, 0, SEEK_END);
	graphBufferStart = ftell(f);
	rewind(f);
	graphBuffer = new unsigned char[graphBufferStart];
	fread(graphBuffer, 1, graphBufferStart, f);
	fclose(f);

	f = fopen(offsetsFileName, "rb");
	fread(&tilesAmount, 4, 1, f);
	fread(&tilesInARow, 4, 1, f);
	fread(&nodesAmount, 4, 1, f);
	xOffsets = new int[tilesAmount+1];
	yOffsets = new int[tilesAmount+1];
	xFirstOffsets = new int[tilesInARow+1];
	yFirstOffsets = new int[tilesInARow+1];
	fread(xOffsets, sizeof(int), tilesAmount+1, f);
	fread(yOffsets, sizeof(int), tilesAmount+1, f);
	fread(xFirstOffsets, sizeof(int), tilesInARow+1, f);
	fread(yFirstOffsets, sizeof(int), tilesInARow+1, f);
	fclose(f);
	
	outArr = new int[nodesAmount];
	tileForUncompress.setParams(tileSize, stripSize, logTileSize, graphBuffer, outArr);
	return true;

}

void CStripeGraph::saveToFile(const char *graphFileName, const char *offsetsFileName) {

	FILE *f = fopen(graphFileName, "wb");
	fwrite(graphBuffer, 1, graphBufferStart, f);
	fclose(f);

	f = fopen(offsetsFileName, "wb");
	fwrite(&tilesAmount, 4, 1, f);
	fwrite(&tilesInARow, 4, 1, f);
	fwrite(&nodesAmount, 4, 1, f);
	fwrite(xOffsets, sizeof(int), tilesAmount+1, f);
	fwrite(yOffsets, sizeof(int), tilesAmount+1, f);
	fwrite(xFirstOffsets, sizeof(int), tilesInARow+1, f);
	fwrite(yFirstOffsets, sizeof(int), tilesInARow+1, f);
	fclose(f);
	
}

int CStripeGraph::successorArray(int node, int **sucArrPtr) {
	int yOffset = (int)(node >> logTileSize);
	int offsetIndex = xFirstOffsets[yOffset], offsetEndIndex = xFirstOffsets[yOffset+1];
	tileForUncompress.setY(node - (yOffset << logTileSize));
	int offset = xOffsets[offsetIndex], nextOffset = xOffsets[offsetIndex+1];
	while(offsetIndex < offsetEndIndex) {
		tileForUncompress.decompressX(offset, nextOffset);
		offsetIndex++;
		offset = nextOffset;
		nextOffset = xOffsets[offsetIndex+1];
	}
	(*sucArrPtr) = outArr;
	return tileForUncompress.getLen();
}

int CStripeGraph::predecessorArray(int node, int **preArrPtr) {
	int xOffset = (int)(node >> logTileSize);
	int offsetIndex = yFirstOffsets[xOffset], offsetEndIndex = yFirstOffsets[xOffset+1];
	tileForUncompress.setX(node - (xOffset << logTileSize));
	int offset = yOffsets[offsetIndex];
	while(offsetIndex < offsetEndIndex) {
		tileForUncompress.decompressY(xOffsets[offset], xOffsets[offset+1]);
		offset = yOffsets[++offsetIndex];
	}
	(*preArrPtr) = outArr;
	return tileForUncompress.getLen();
}

int CStripeGraph::size() {
	return graphBufferStart + ((tilesAmount+1)<<1)*sizeof(int) + ((tilesInARow+1)<<1)*sizeof(int) + 12;
}