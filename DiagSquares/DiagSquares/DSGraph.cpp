#include "DSGraph.h"
#include <cstdio>

DSGraph::DSGraph() {
	tiles = 0;
	graphBuffer = 0;
	offsets = 0;
	sucArr = 0;
	tileAmount = -1;
}

DSGraph::~DSGraph() {
	delete[] graphBuffer, tiles, offsets, sucArr;
}

int DSGraph::getTileNr(int x, int y, int *oX, int *oY) {
	if((x==0) && (y==0))
		return 0;
	int log = 0;
	while((x >> log) != 0) log++;
	while((y >> log) != 0) log++;
	log--;
	int ret = 2 << log;
	ret = (ret << 1) + ret - 2;
	if((x >> log) == 0) { // left lower square
		(*oY) += 1 << log;
		return ret - 1;
	}
	if((y >> log) == 0) { // right upper square
		(*oX) += 1 << log;
		return ret - 2;
	}
	(*oX) += 1 << log;
	(*oY) += 1 << log;
	return ((ret - 2) >> 1) + getTileNr( x - (1 << log), y - (1 << log), oX, oY ); // lower right square
}

void DSGraph::loadFromTextFile(const char *textFileName) {

	// try to open file
	FILE *f = fopen(textFileName, "rb");
	if(f==NULL) {
		printf("Cannot open file %s.\n", textFileName);
		return;
	}

	long long fileSize;
	// check file size
	_fseeki64(f, 0, SEEK_END);
	fileSize = _ftelli64(f);
	rewind(f);

	// allocate memory for read
	unsigned char *buffer = new unsigned char[fileSize];
	if(buffer == NULL) {
		printf("Memory allocation error (%lld bytes wanted).\n", fileSize);
		fclose(f);
		return;
	}

	// read and close file
	fread(buffer, sizeof(unsigned char), fileSize, f);
	fclose(f);

	// get number of lines (amount of nodes)
	nodesAmount = 0;
	for(long long i=0; i<fileSize; i++)
		if(buffer[i] == '\n')
			nodesAmount++;

	// compute values (tileSize and stripSize must be a power of 2)
	level = 0;
	tileAmount = 1;
	for(int i=1; nodesAmount > i; i<<=1) {
		tileAmount = (tileAmount << 1) + 2;
		level++;
	}

	printf(" level %d, tileAmount %d, nodesAmount %d\n", level, tileAmount, nodesAmount);

	// set sizes of tiles
	tiles = new CUncompressedTile[tileAmount];
	tiles[0].setSizes(0);
	int t = 1, s = 0;
	for(int l=0; l<level; l++) {
		// copy sizes
		for(int i=0; i<t; i++)
			tiles[t+i].setSizes( tiles[i].getSize() );
		// add two at the end
		t <<= 1;
		tiles[t].setSizes(s);
		tiles[t+1].setSizes(s);
		t += 2;
		s++;
	}

	// process all lines
	long long ind = 0;
	int listSize, tmp, oX, oY;
	maxListSize = 0;
	for(int i=0; i<nodesAmount; i++) {

		listSize = 0;

		// add all the edges to the corresponding tile
		tmp = 0;
		while(buffer[ind] != '\n') {
			if(buffer[ind] == ' ') {
				// add to tile ( x=tmp, y=i )
				oX = oY = 0;
				tiles[getTileNr(tmp, i, &oX, &oY)].addLink(tmp-oX, i-oY);
				tmp = 0;
				listSize++;
			} else tmp = (tmp<<3) + (tmp<<1) + buffer[ind] - '0';
			ind++;
		}
		ind++;

		if(listSize > maxListSize)
			maxListSize = listSize;

		if((i >> 14) << 14 == i) printf(". ");

	}

	graphBuffer = buffer; // we don't need buffer anymore, so why not use it?
	graphBufferStart = 0;
	offsets = new unsigned int[tileAmount+1];
	maxTileElements = 0;

	std::set<std::pair<unsigned int, std::pair<int, int>> > temp;
	int compress[4] = { 0, 0, 0, 0 };
	// compress tiles
	for(int i=0; i<tileAmount; i++) {
		if((i >> 14) << 14 == i) printf(", ");
		if(tiles[i].isNotEmpty()) {
			if((int)tiles[i].getSize() > maxTileElements)
				maxTileElements = tiles[i].getSize();
			unsigned int size = tiles[i].getSize();
			offsets[i] = tiles[i].compress(buffer, graphBufferStart);

			temp.insert(std::make_pair(size, std::make_pair(i, offsets[i] >> 30)));
			compress[offsets[i] >> 30]++;

		} else offsets[i] = 1 << 30;
	}
	for(int i=0; i<4; i++) printf("%d ", compress[i]); printf("\n");

	int counter = 1;
	for(auto it=temp.rbegin(); it!=temp.rend(); it++) {
		if(counter++ == 50)
			break;
		printf("%u edges in tile %d %s, %s\n", it->first, it->second.first, (it->second.second & 1) ? "zipped" : "no zip", (it->second.second >> 1) ? "transposed" : "normal");
	}
	
	offsets[tileAmount] = graphBufferStart; // we need to know the length of the last chunk

	for(int i=tileAmount-1; i>-1; i--)
		if(offsets[i] == (1 << 30))
			offsets[i] = offsets[i+1];

	delete[] tiles, sucArr;
	printf("\n");

	sucArr = new int[maxTileElements];
	tileForSuc.setParams(graphBuffer, sucArr, &sucLen, maxTileElements);

}

void DSGraph::loadFromFile(const char *graphFileName, const char *offsetsFileName) {

	FILE *f = fopen(graphFileName, "rb");
	fseek(f, 0, SEEK_END);
	graphBufferStart = ftell(f);
	rewind(f);
	delete[] graphBuffer, offsets, sucArr;
	graphBuffer = new unsigned char[graphBufferStart];
	fread(graphBuffer, 1, graphBufferStart, f);
	fclose(f);

	f = fopen(offsetsFileName, "rb");
	fread(&level, 4, 1, f);
	fread(&tileAmount, 4, 1, f);
	fread(&maxListSize, 4, 1, f);
	fread(&maxTileElements, 4, 1, f);
	offsets = new unsigned int[tileAmount+1];
	fread(offsets, sizeof(unsigned int), tileAmount+1, f);
	fclose(f);

	sucArr = new int[maxTileElements];
	tileForSuc.setParams(graphBuffer, sucArr, &sucLen, maxTileElements);

}

void DSGraph::saveToFile(const char *graphFileName, const char *offsetsFileName) {

	FILE *f = fopen(graphFileName, "wb");
	fwrite(graphBuffer, 1, graphBufferStart, f);
	fclose(f);

	f = fopen(offsetsFileName, "wb");
	fwrite(&level, 4, 1, f);
	fwrite(&tileAmount, 4, 1, f);
	fwrite(&maxListSize, 4, 1, f);
	fwrite(&maxTileElements, 4, 1, f);
	fwrite(offsets, sizeof(unsigned int), tileAmount+1, f);
	fclose(f);

}

unsigned int DSGraph::successorArray(int row, int **sucArrPtr) {
	sucLen = 0;
	int log = level-1;
	int ind = tileAmount;
	int indOff = 0;
	int offX = 0;
	while(log != -1) {
		if((row >> log) == 0) { // upper half

			tileForSuc.decompressX(log, offX+(1<<log), row, offsets[indOff+ind-2], offsets[indOff+ind-1]); // decompress upper right
			ind = (ind - 2) >> 1;
			log--;
		} else { // lower half
			row -= 1 << log;
			tileForSuc.decompressX(log, offX, row, offsets[indOff+ind-1], offsets[indOff+ind]); // decompress lower left
			ind = (ind - 2) >> 1;
			indOff += ind;
			offX += 1 << log;
			log--;
		}
	}
	tileForSuc.decompressX(0, offX, 0, offsets[indOff], offsets[indOff+1]); // decompress remaining tile
	(*sucArrPtr) = sucArr;
	return sucLen;
}
