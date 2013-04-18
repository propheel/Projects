#define OS_WIN /* We're using MSVC. Another options *will* be added :) */
#define _CRT_SECURE_NO_WARNINGS
#define _FILE_OFFSET_BITS 64
#include <cstdlib>
#include <cstdio>
#include <string>
#include <time.h>
#include "boost/filesystem.hpp"
#include "asmlib.h"

#define FASTQ_READ_BUFFER 67108864 /* 64 MB */
#define WRITE_BUCKET_INITIAL_BUFFER 1000 /* number of elements in a default size bucket buffer */
#define MAX_OPENED_FILES 2010
#define HASH_TABLE_INITIAL_SIZE 1048576 /* 2^20 elements initial hash table size */
#define FASTQ_WRITE_BUFFER 33554432 /* 32 MB */
#define OUTPUT_READ_INDEX_DIGITS 20 /* number of digits of the index number */
#define HEADER_ENDING ":4:1:8:1030:\n" /* ending of a fastq header for merged reads by Fulcrum */
#define QUALITY_STANDARD 33 /* quality is an ASCII of Phred + QUALITY_STANDARD */
#define QUALITY_THRESHOLD 60 /* max possible quality score (0 - 60) according to Fulcrum */

namespace bfs = boost::filesystem3;

class CResizableWriteBuffer;

class COpenedFiles {
	CResizableWriteBuffer **heap;
	unsigned int size;
public:
	COpenedFiles() {
		heap = NULL;
		size = 0;
	}
	~COpenedFiles();
	bool allocate();
	void checkForFilesAndClose();
	void addFile(CResizableWriteBuffer *buf);
	void updateFile(CResizableWriteBuffer *buf, unsigned int pos);
};


////////////////////////////////////////////////////////////////////////////////////
// RESIZABLE BUFFER DEFINITION

class CResizableWriteBuffer {
	FILE *f; // file handle
	unsigned long long pos, size; // position in buffer and size of buffer in bytes
	unsigned int entryLen, entries, totalEntries, heapPos; // number of entries and total number of entries (after all flushes)
	char *buffer; // buffer itself
	char *fName; // file name (useful for faster opening)
public:
	CResizableWriteBuffer() {
		pos = size = entryLen = entries = totalEntries = heapPos = 0;
		buffer = fName = NULL;
		f = NULL;
	}
	~CResizableWriteBuffer() {
		flushAndClose();
		if(buffer!=NULL) free(buffer);
		if(fName!=NULL) free(fName);
	}
	bool allocate(const char *fileName, unsigned int readLen) {
		entryLen = readLen;
		// try to copy file name
		fName = (char*)malloc(A_strlen(fileName)+1);
		if(fName == NULL) return false;
		A_strcpy(fName, fileName);
		// try to allocate memory for buffer
		size = WRITE_BUCKET_INITIAL_BUFFER*readLen;
		buffer = (char*)malloc(size);
		return (buffer != NULL);
	}
	void parse(char *entry, char *quality, COpenedFiles *opFiles, unsigned long long *reallocatedMemory) {
		if(pos + (entryLen<<1) >= size) { // flush
			totalEntries += entries;
			if(f == NULL) { // open file
				opFiles->checkForFilesAndClose();
				f = fopen(fName, "a");
				if(f == NULL) {
					printf("Error: Cannot open file %s.\n", fName);
					return;
				}
				opFiles->addFile(this);
			} else opFiles->updateFile(this, heapPos);
			fwrite(buffer, pos, 1, f);
			if(*reallocatedMemory >= size) {
				char *newB = (char*)malloc(size<<1); // try to allocate more memory
				if(newB != NULL) {
					free(buffer);
					buffer = newB;
					(*reallocatedMemory) -= size;
					size <<= 1;
				}
			}
			pos = entries = 0;
		}
		A_memcpy(buffer+pos, entry, entryLen);
		pos += entryLen;
		A_memcpy(buffer+pos, quality, entryLen);
		pos += entryLen;
		entries++;
	}
	unsigned int getEntries() { return totalEntries; }
	void setHeapPos(unsigned int pos) { heapPos = pos; }
	void flushAndClose() {
		if(pos!=0) {
			if(f == NULL) {
				f = fopen(fName, "a");
				if(f == NULL) {
					printf("Error: Cannot open file %s.\n", fName);
					return;
				}
			}
			fwrite(buffer, pos, 1, f);
			totalEntries += entries;
			pos = entries = 0;
		}
		if(f!=NULL) {
			fclose(f);
			f = NULL;
		}
		heapPos = 0;
	}
};


////////////////////////////////////////////////////////////////////////////////////
// PRIORITY QUEUE OF OPENED FILES DEFINITION

COpenedFiles::~COpenedFiles() {
	if(heap!=NULL) {
		if(heap[0]!=NULL)
			delete heap[0];
		free(heap);
	}
}

bool COpenedFiles::allocate() {
	heap = (CResizableWriteBuffer**)malloc(MAX_OPENED_FILES*sizeof(CResizableWriteBuffer*));
	if(heap==NULL) return false;
	heap[0] = new CResizableWriteBuffer();
	return true;
}

void COpenedFiles::checkForFilesAndClose() { // check if we can open more files. if not close the one with least reads in total
	unsigned int i, child;
	if(size >= MAX_OPENED_FILES) {
		heap[1]->flushAndClose();
		CResizableWriteBuffer *last = heap[size--];
		for(i = 1; (i<<1) <= size; i = child) {
			child = i<<1;
			if((child!=size) && (heap[child+1]->getEntries() < heap[child]->getEntries()))
				child++;
			if(last->getEntries() > heap[child]->getEntries()) {
				heap[i] = heap[child];
				heap[i]->setHeapPos(i);
			} else
				break;
		}
		heap[i] = last;
		last->setHeapPos(i);
	}
}

void COpenedFiles::addFile(CResizableWriteBuffer *buf) {
	heap[++size] = buf;
	updateFile(buf, size);
}

void COpenedFiles::updateFile(CResizableWriteBuffer *buf, unsigned int pos) {
	for(; heap[pos>>1]->getEntries() > buf->getEntries(); pos>>=1)
		heap[pos] = heap[pos>>1];
	heap[pos] = buf;
	buf->setHeapPos(pos);
}


////////////////////////////////////////////////////////////////////////////////////
// CHECKING FASTQ FILE FORMAT

void identifyReadLengths(unsigned int *minHeaderLen, unsigned int *readLen, char *newLineChar, bool *winNewLine, bool *exactFooter, unsigned long long *fileSize, FILE *fIn) {

// check file size
#ifdef OS_WIN
	_fseeki64(fIn, 0, SEEK_END);
	*fileSize = _ftelli64(fIn);
#elif defined OS_LINUX
	fseeko(fIn, 0, SEEK_END);
	*fileSize = ftello(fIn);
#endif
	rewind(fIn);

// check lengths
	*minHeaderLen = 6; // there is a word 'length' in a header
	char c;
	for(fread(&c, 1, 1, fIn); ((c!='\n') && (c!='\r')); fread(&c, 1, 1, fIn))
		if((c=='@')||(c=='.')||(c==' ')||(c==':')||(c=='='))
			(*minHeaderLen)+=2;
	(*minHeaderLen)++; // new line
	*newLineChar = c; // can be \r for Macs (still little chance it will be run on Mac)
	fread(&c, 1, 1, fIn);
	if(c == '\n') {
		*winNewLine = true;
		*readLen = 0;
	} else {
		*winNewLine = false;
		*readLen = 1;
	}
	for(fread(&c, 1, 1, fIn); c!=*newLineChar; fread(&c, 1, 1, fIn)) (*readLen)++;
	if(*winNewLine) fread(&c, 1, 1, fIn);
	fread(&c, 1, 1, fIn); // read '+'
	fread(&c, 1, 1, fIn);
	*exactFooter = (c != *newLineChar);

	rewind(fIn);

}


////////////////////////////////////////////////////////////////////////////////////
// CREATE CHAR CHANGE TABLE

unsigned int* createChaChaTable(unsigned int prefix_len, unsigned int *pow5) {
	unsigned int *chacha = (unsigned int*)malloc(5*128*prefix_len*sizeof(unsigned int));
	if(chacha == NULL) return NULL;
	for(int i=prefix_len-1; i!=-1; i--) {
		chacha['A'+(i<<7)] = 0;
		chacha['C'+(i<<7)] = *pow5;
		chacha['G'+(i<<7)] = (*pow5)<<1;
		chacha['T'+(i<<7)] = ((*pow5)<<1)+(*pow5);
		chacha['N'+(i<<7)] = (*pow5)<<2;
		*pow5 = ((*pow5)<<2)+(*pow5); // pow5*5
	}
	return chacha;
}

////////////////////////////////////////////////////////////////////////////////////
// CUT FASTQ FILE INTO SMALLER BUCKETS

int cutFastq(unsigned int prefix_len, unsigned long long reallocatedMemory, FILE *fIn, char *outFolder, unsigned int &returnReadLen) {

// check max opened files limit
	if(_setmaxstdio(MAX_OPENED_FILES+10) != MAX_OPENED_FILES+10) {
		printf("Couldn't change opened files limit.\n");
		return 1;
	}

// identify read file (possibly we can add support for different read and quality formats, for now it's ACGTN and Phred+33)
	char newLineChar;
	bool winNewLine, exactFooter;
	unsigned int minHeaderLen, readLen;
	unsigned long long fileSize;

	identifyReadLengths(&minHeaderLen, &readLen, &newLineChar, &winNewLine, &exactFooter, &fileSize, fIn);
	returnReadLen = readLen;

// try to create buffers
	char *read_buffer = (char*)malloc(FASTQ_READ_BUFFER);
	unsigned int pow5 = 1;
	unsigned int *chacha = createChaChaTable(prefix_len, &pow5);
	CResizableWriteBuffer **write_buffers = (CResizableWriteBuffer**)malloc(pow5*sizeof(CResizableWriteBuffer*));
	if((read_buffer == NULL) || (chacha == NULL) || (write_buffers == NULL)) {
		printf("Memory allocation error.\n");
		return 2;
	}

// create file names and write buffers
	std::string str;
	str.assign(outFolder);
	if((str[str.length()-1] != '\\') && (str[str.length()-1] != '/'))
		str.append("/");
	for(unsigned int i=0; i<prefix_len; i++) str.append("A");
	for(unsigned int i=0; i+1<pow5; i++) {
		write_buffers[i] = new CResizableWriteBuffer();
		if((write_buffers[i] == NULL) || (!write_buffers[i]->allocate(str.c_str(), readLen))) {
			printf("Memory allocation error.\n");
			return 2;
		}
		size_t pos = str.length()-1;
		bool need = true;
		while(need) {
			switch(str[pos]) {
				case 'A' : str[pos] = 'C'; need = false; break;
				case 'C' : str[pos] = 'G'; need = false; break;
				case 'G' : str[pos] = 'T'; need = false; break;
				case 'T' : str[pos] = 'N'; need = false; break;
				case 'N' : str[pos] = 'A'; pos--; break;
			}
		}
	}
	write_buffers[pow5-1] = new CResizableWriteBuffer();
	if((write_buffers[pow5-1] == NULL) || (!write_buffers[pow5-1]->allocate(str.c_str(), readLen))) {
		printf("Memory allocation error.\n");
		return 2;
	}

// initialize opened files priority queue
	COpenedFiles openedFiles;
	if(!openedFiles.allocate()) {
		printf("Memory allocation error.\n");
		return 2;
	}

// main loop
	unsigned long long position, finalPos, bytes_read, bytes_read_total = 0;
	unsigned int minFooterLen = exactFooter ? minHeaderLen : 1;
	unsigned int newLines = winNewLine ? 2 : 1;
	while(bytes_read_total < fileSize) {
		if(fileSize - bytes_read_total < FASTQ_READ_BUFFER)
			bytes_read = fread(read_buffer, sizeof(char), fileSize-bytes_read_total, fIn);
		else
			bytes_read = fread(read_buffer, sizeof(char), FASTQ_READ_BUFFER, fIn);
		for(position = 0, finalPos = 0; position + (minHeaderLen+(readLen<<1)+minFooterLen+(newLines<<2)) <= bytes_read; ) {
			position += minHeaderLen-1;
			bool need = true; // we need \n
			while((need) && (position < bytes_read)) if(read_buffer[position++] == newLineChar) need = false;
			if(winNewLine) position++;
			unsigned int footerSize = exactFooter ? (unsigned int)(position - finalPos) : 1;
			if((position + footerSize + (readLen<<1) + (newLines<<1)) <= bytes_read) { // let's see if it's worth it
				unsigned int fileId = 0;
				for(unsigned int i=0; i<prefix_len; i++)
					fileId += chacha[read_buffer[position+i]+(i<<7)];
				write_buffers[fileId]->parse(&read_buffer[position], &read_buffer[position+readLen+newLines+footerSize], &openedFiles, &reallocatedMemory);
				position += footerSize + ((readLen + newLines)<<1);
				finalPos = position;
			} else position = bytes_read;
		}
		bytes_read_total += finalPos;
#ifdef OS_WIN
		_fseeki64(fIn, bytes_read_total, SEEK_SET);
#elif defined OS_LINUX
		fseeko(fIn, bytes_read_total, SEEK_SET);
#endif
		printf("%llu bytes read.\n", bytes_read_total);
	}

// free buffers
	free(read_buffer);
	free(chacha);
	for(unsigned int i=0; i<pow5; i++) delete write_buffers[i]; // here all the rest will be flushed by destructors
	free(write_buffers);
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////
// JUMP LIST DEFINITION

class CJumpList {
	unsigned long long *jumpList, *jumpListEnds;
public:
	CJumpList() {
		jumpList = jumpListEnds = NULL;
	}
	~CJumpList() {
		if(jumpList!=NULL) free(jumpList);
		if(jumpListEnds!=NULL) free(jumpListEnds);
	}
	bool allocate(unsigned long long size) {
		jumpList = (unsigned long long*)malloc(size*sizeof(unsigned long long));
		jumpListEnds = (unsigned long long*)malloc(size*sizeof(unsigned long long));
		return ((jumpList!=NULL) && (jumpListEnds!=NULL));
	}
	bool reallocate(unsigned long long size) {
		if(jumpList!=NULL) free(jumpList);
		if(jumpListEnds!=NULL) free(jumpListEnds);
		return allocate(size);
	}
	void add(unsigned long long indexingReadNum, unsigned long long readNum) {
		unsigned long long tmp = jumpListEnds[indexingReadNum];
		jumpListEnds[indexingReadNum] = readNum;
		jumpList[tmp] = readNum;
		jumpList[readNum] = -1;
	}
	void newEntry(unsigned long long readNum) {
		jumpListEnds[readNum] = readNum;
		jumpList[readNum] = -1;
	}
	unsigned long long get(unsigned long long index) { return jumpList[index]; }
};

////////////////////////////////////////////////////////////////////////////////////
// HASH TABLE DEFINITION

struct SHashValue { // char * = 8B, ull = 8B -> 16B structure
	char *read;
	unsigned long long read_num;
};

class CHashTable {
	SHashValue *table;
	unsigned int entries, size, prefix_len, k, readLen, *goToTable; // goToTable stores indices of non-NULL entries in hash table (its length = entries)
public:
	CHashTable(unsigned int prefix_len=0, unsigned int k=0, unsigned int readLen=0) {
		this->prefix_len = prefix_len;
		this->k = k;
		this->readLen = readLen;
		entries = size = 0;
		table = NULL;
		goToTable = NULL;
	}
	~CHashTable() {
		if(table!=NULL) free(table);
		if(goToTable!=NULL) free(goToTable);
	}
	bool allocate() {
		table = (SHashValue*)malloc((size = HASH_TABLE_INITIAL_SIZE)*sizeof(SHashValue));
		goToTable = (unsigned int*)malloc(size*sizeof(unsigned int));
		if((table==NULL) || (goToTable==NULL)) {
			printf("Memory allocation error.\n");
			return false;
		}
		A_memset(table, 0, size*sizeof(SHashValue)); // it is important to set all pointers to NULL
		return true;
	}
	bool reallocate() {
		size <<= 1;
		SHashValue *newT = (SHashValue*)malloc(size*sizeof(SHashValue)), *tmpT=table;
		unsigned int *newG = (unsigned int*)malloc(size*sizeof(unsigned int));
		if((newT==NULL) || (newG==NULL)) {
			printf("Memory allocation error. [ %u ] \n", size);
			size >>= 1;
			return false;
		}
		table = newT; // this is for findEntry function to work properly
		A_memset(table, 0, size*sizeof(SHashValue)); // it is important to set all pointers to NULL
		for(unsigned int i=0; i<entries; i++) {
			unsigned int htIndex = goToTable[i];
			unsigned int h;
			findEntry(tmpT[htIndex].read, h); // recalculate hashes
			table[h].read = tmpT[htIndex].read;
			table[h].read_num = tmpT[htIndex].read_num;
			newG[i] = h;
		}
		free(tmpT);
		free(goToTable);
		goToTable = newG;
		return true;
	}
	unsigned int hash(char *s) { // Bernstein
		unsigned int h = 0;
		for(unsigned int i=prefix_len; i<k; i++)
			h = (h<<5)+h + s[i];
		return h & (size-1);
	}
	bool findEntry(char *read, unsigned int &h) {
		h = hash(read);
		bool matches = false;
		while((!matches) && (table[h].read!=NULL)) { // find entry in hash table
			matches = true;
			for(unsigned int a=prefix_len; a<k; a++)
				if(table[h].read[a] != *(read+a)) {
					matches = false;
					h = (h == size-1) ? 0 : h+1;
					break;
				}
		}
		return matches;
	}
	long long add(char *read, unsigned long long read_num) { // add read to the jumplist
		unsigned int h;
		if(findEntry(read, h))
			return table[h].read_num;
		else {
			if((entries<<3)+(entries<<1) > (size<<3)-size) { // factor > 0.7
				reallocate();
				return add(read, read_num); // find new position of the entry
			} else {
				goToTable[entries++] = h; // if not then create new entry in table and in the list
				table[h].read = read;
				table[h].read_num = read_num;
				return -1;
			}
		}
	}
	unsigned long long getLastEntry() {
		if(entries==0) return 0;
		unsigned long long res = goToTable[--entries];
		table[res].read = NULL;
		return table[res].read_num;
	}
	bool hasEntries() { return (entries!=0); }
};


////////////////////////////////////////////////////////////////////////////////////
// PERFORM MERGE OF READ WITH ANOTHER ONE FROM ALREADY MERGED READS

void performMerge(char *read_pos, unsigned int readLen, unsigned int *quality_pos, unsigned int k, unsigned int minmismatch, unsigned int nVal, unsigned long long &mergesCount, char **mergesListRead, unsigned int **mergesListQlt) {
	unsigned long long group = -1;
// make a table for checking the position that have quality > NVAL
	unsigned int *nxtCheck = (unsigned int*)malloc(readLen*sizeof(unsigned int));
	unsigned int nxtChkCnt = 0;
	for(unsigned int a=k; a<readLen; a++)
		if(*(quality_pos+a) > nVal)
			nxtCheck[nxtChkCnt++] = a;
	nxtCheck[nxtChkCnt] = readLen;
// here we go (n^2-n)/2
	for(unsigned long long i=0; i<mergesCount; i++) {
		unsigned int mismatches = 0;
		char *merged_read_pos = mergesListRead[i];
		unsigned int *merged_quality_pos = mergesListQlt[i];
	// compare reads and check qualities
		for(unsigned int a=nxtCheck[0], j=0; a<readLen; a=nxtCheck[++j])
			if ((*(merged_read_pos+a) != *(read_pos+a) && (*(merged_quality_pos+a) > nVal))) {
				if(++mismatches >= minmismatch)
					break;
			}
		if(mismatches<minmismatch) {
			group = i;
			break;
		}
	}
	if(group!=-1) { // if the read matches to another one, merge it
		char *merged_read_pos = mergesListRead[group];
		unsigned int *merged_quality_pos = mergesListQlt[group];
		for(unsigned int a=0; a<readLen; a++) {
			if(*(merged_read_pos+a) == *(read_pos+a)) {
				*(merged_quality_pos+a) += *(quality_pos+a);
			} else {
				if(*(merged_quality_pos+a) > *(quality_pos+a)) {
					*(merged_quality_pos+a) -= *(quality_pos+a);
				} else {
					if(*(merged_quality_pos+a) == *(quality_pos+a)) {
						*(merged_quality_pos+a) = 0;
						*(merged_read_pos+a) = 'N';
					}
				}
				if(*(merged_quality_pos+a) < *(quality_pos+a)) {
					*(merged_quality_pos+a) = *(quality_pos+a) - *(merged_quality_pos+a);
					*(merged_read_pos+a) = *(read_pos+a);
				}
			}
		}
	} else { // if not, create another merge
		mergesListRead[mergesCount] = read_pos;
		mergesListQlt[mergesCount++] = quality_pos;
	}
	free(nxtCheck);
}


////////////////////////////////////////////////////////////////////////////////////
// ADD MERGED READ TO WRITE BUFFER

void writeMergedReads(unsigned long long &wb_size, char* &write_pos, unsigned long long &wb_capacity, char *read_index, unsigned int &read_index_len, unsigned int readLen, char *read_pos, unsigned int *quality_pos, char *write_buffer, FILE *fout) {
	if(wb_size + 18 + (readLen<<1) + read_index_len > wb_capacity) { // flush output
		// TODO maybe reallocate write buffer ...
		fwrite(write_buffer, sizeof(char), wb_size, fout);
		wb_size = 0;
		write_pos = write_buffer;
	}
	*(write_pos++) = '@'; // start header
	A_memcpy(write_pos, read_index + (OUTPUT_READ_INDEX_DIGITS-read_index_len), read_index_len*sizeof(char)); // put index number
	write_pos += read_index_len;
	A_memcpy(write_pos, HEADER_ENDING, strlen(HEADER_ENDING)); // put header ending and new line
	write_pos += strlen(HEADER_ENDING);
	A_memcpy(write_pos, read_pos, readLen); // put read
	write_pos += readLen;
	A_memcpy(write_pos, "\n+\n", 3); // put new line and second (optional) header '+'
	write_pos += 3;
	for(unsigned int j=0; j<readLen; j++) // cut qualities over maximum and put them
		if(*(quality_pos+j) < QUALITY_THRESHOLD)
			*(write_buffer + wb_size + j) = *(quality_pos+j) + QUALITY_STANDARD;
		else
			*(write_buffer + wb_size + j) = QUALITY_THRESHOLD + QUALITY_STANDARD;
	write_pos += readLen;
	*(write_pos) = '\n'; // put a new line
	write_pos++;
	wb_size += read_index_len+5+strlen(HEADER_ENDING)+(readLen<<1);
// increase index number
	for(int j=OUTPUT_READ_INDEX_DIGITS-1; j>-1; j--)
		if(read_index[j] == '9') {
			read_index[j] = '0';
			if(OUTPUT_READ_INDEX_DIGITS-j == read_index_len) {
				read_index_len++;
				read_index[OUTPUT_READ_INDEX_DIGITS-read_index_len] = '0';
			}
		} else {
			read_index[j]++;
			break;
		}
}

////////////////////////////////////////////////////////////////////////////////////
// MERGE READS FROM BUCKETS INTO ONE FILE

int mergeFiles(unsigned int prefix_len, unsigned int k, unsigned int min_mismatch, unsigned int nval, unsigned int readLen, char *resultFolder, FILE *fOut) {

// allocate memory
	CHashTable hTable(prefix_len, k, readLen);
	CJumpList jList;
	unsigned int pow5 = 1;
	unsigned long long rb_size = FASTQ_READ_BUFFER; // Could be another MERGE_READ_BUFFER, but what for?
	char *read_buffer = (char*)malloc(rb_size);
	unsigned int *qualities = (unsigned int*)malloc((rb_size>>1)*sizeof(unsigned int));
	unsigned long long wb_size=0, wb_capacity = FASTQ_WRITE_BUFFER;
	char *write_buffer = (char*)malloc(wb_capacity);
	char *wb_pos = write_buffer;
	char **mergesListRead = (char**)malloc(((rb_size/readLen)>>1)*sizeof(char*));
	unsigned int **mergesListQlt = (unsigned int**)malloc(((rb_size/readLen)>>1)*sizeof(char*));
	if((!hTable.allocate()) || (!jList.allocate((rb_size/readLen)>>1)) || (read_buffer==NULL) || (write_buffer==NULL) || (qualities==NULL) || (mergesListRead==NULL) || (mergesListQlt==NULL)) {
		printf("Memory allocation error.\n");
		return 2;
	}

	char read_index[OUTPUT_READ_INDEX_DIGITS];
	unsigned int read_index_len = 1;
	read_index[OUTPUT_READ_INDEX_DIGITS-1] = '1';

// start browsing through files
	for(bfs::directory_iterator inDir(resultFolder); inDir!=bfs::directory_iterator(); inDir++) {

	// read file
		FILE *fin = fopen(inDir->path().string().c_str(), "r");
		if(fin == NULL) { printf("Error reading file.\n"); return 3; }
		unsigned long long fSize;
#ifdef OS_WIN
		_fseeki64(fin, 0, SEEK_END);
		fSize = _ftelli64(fin);
#elif defined OS_LINUX
		fseeko(fin, 0, SEEK_END);
		fSize = ftello(fin);
#endif
		rewind(fin);
	// reallocate memory for read if needed
		if(fSize > rb_size) {
			free(read_buffer);
			free(qualities);
			free(mergesListRead);
			free(mergesListQlt);
			rb_size = fSize;
			read_buffer = (char*)malloc(rb_size);
			qualities = (unsigned int*)malloc((rb_size>>1)*sizeof(unsigned int));
			mergesListRead = (char**)malloc(((rb_size/readLen)>>1)*sizeof(char*));
			mergesListQlt = (unsigned int**)malloc(((rb_size/readLen)>>1)*sizeof(char*));
			if((read_buffer==NULL) || (qualities==NULL) || (!jList.reallocate((fSize/readLen)>>1))) {
				printf("Memory allocation error.\n");
				return 2;
			}
		}
		fread(read_buffer, sizeof(char), fSize, fin);
		fclose(fin);

	// add to hash table
		unsigned long long read_number = 0;
		unsigned int *quality_pos = qualities;
		char *read_pos = read_buffer;
		char *buffer_end = read_buffer + fSize;
		while(read_pos < buffer_end) {
			for(unsigned int i=readLen; i!=(readLen<<1); i++) *(quality_pos++) = *(read_pos+i) - 33; // change qualities from Phred+33
			long long res = hTable.add(read_pos, read_number);
			if(res == -1)
				jList.newEntry(read_number);
			else
				jList.add(res, read_number);
			read_pos += (readLen<<1);
			read_number++;
		}

		unsigned long long merges = 0;
		std::clock_t timer = std::clock();
	// iterate through every k-mer of hash table
		while(hTable.hasEntries()) {
			unsigned long long mergesCount = 0;
		// merge reads
			read_number = hTable.getLastEntry();
			for(; read_number!=-1; read_number=jList.get(read_number)) {
				unsigned long long offset = read_number*readLen;
				performMerge(read_buffer+(offset<<1), readLen, qualities+offset, k, min_mismatch+1, nval, mergesCount, mergesListRead, mergesListQlt);
			}
			merges += mergesCount;
		// write merged reads to file
			for(unsigned long long i=0; i<mergesCount; i++) {
				std::clock_t writeTimer = std::clock();
				writeMergedReads(wb_size, wb_pos, wb_capacity, read_index, read_index_len, readLen, mergesListRead[i], mergesListQlt[i], write_buffer, fOut);
				timer += std::clock()-writeTimer;
			}
		}
		printf("%s %llu merges [ %.2lf ms ]\n", inDir->path().string().c_str(), merges, (double)(std::clock()-timer)*1000.0/CLOCKS_PER_SEC);
	}

// flush rest of output
	if(wb_size != 0)
		fwrite(write_buffer, sizeof(char), wb_size, fOut);
	printf("Total number of output reads: ");
	for(unsigned int i=OUTPUT_READ_INDEX_DIGITS-read_index_len; i<OUTPUT_READ_INDEX_DIGITS; i++) printf("%c", read_index[i]);
	printf("-1\n");

// free buffers
	free(read_buffer);
	free(qualities);
	free(write_buffer);
	free(mergesListRead);
	free(mergesListQlt);
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////
// MAIN

int main(int argc, char* argv[]) {

	unsigned long long memoryLimit;
	unsigned int k, l, m, n, readLen;
	char *resultFolder;
	FILE *fFastq, *fResult;
	bool badParams = false;

// TODO: zrób tu obs³ugê parametrów PORZ¥DNIE!
	// na razie jest: c prefixLen memLimit fastqFile resultFolder
	// oraz: p prefixLen k minMismatch nVal readLen resultFolder resultFastq
	// plus: cp prefixLen memLimit k minMismatch nVal fastqFile resultFolder resultFastq

	if(argc < 2) badParams = true; else
		if((strcmp(argv[1], "c")==0) && (argc==6)) {
			fResult = NULL;
			fFastq = fopen(argv[4], "r");
			if(fFastq==NULL) {
				printf("Error: Cannot open %s.\n", argv[4]);
				return 1;
			}
			boost::system::error_code ec;
			if(bfs::exists(argv[5], ec))
				bfs::remove_all(argv[5], ec);
			bfs::create_directory(argv[5], ec);
			resultFolder = argv[5];
			l = atoi(argv[2]);
#ifdef OS_WIN
			memoryLimit = _atoi64(argv[3]);
#elif defined OS_LINUX
			memoryLimit = atoq(argv[3]);
#endif
			// TODO check limits
		} else if((strcmp(argv[1], "p")==0) && (argc==9)) {
			fFastq = NULL;
			fResult = fopen(argv[8], "w");
			if(fResult==NULL) {
				printf("Error: Cannot open %s.\n", argv[8]);
				return 1;
			}
			boost::system::error_code ec;
			if(!bfs::exists(argv[7], ec)) {
				printf("Error: Directory %s does not exist.\n", argv[7]);
				return 2;
			}
			resultFolder = argv[7];
			l = atoi(argv[2]);
			k = atoi(argv[3]);
			m = atoi(argv[4]);
			n = atoi(argv[5]);
			readLen = atoi(argv[6]);
		} else if((strcmp(argv[1], "cp")==0) && (argc==10)) {
			fFastq = fopen(argv[7], "r");
			if(fFastq==NULL) {
				printf("Error: Cannot open %s.\n", argv[4]);
				return 1;
			}
			boost::system::error_code ec;
			if(bfs::exists(argv[8], ec))
				bfs::remove_all(argv[8], ec);
			bfs::create_directory(argv[8], ec);
			resultFolder = argv[8];
			l = atoi(argv[2]);
#ifdef OS_WIN
			memoryLimit = _atoi64(argv[3]);
#elif defined OS_LINUX
			memoryLimit = atoq(argv[3]);
#endif
			// TODO check limits
			fResult = fopen(argv[9], "w");
			if(fResult==NULL) {
				printf("Error: Cannot open %s.\n", argv[8]);
				return 1;
			}
			k = atoi(argv[4]);
			m = atoi(argv[5]);
			n = atoi(argv[6]);
		} else badParams = true;
	if(badParams) {
		printf("Error: Wrong parameters.\nUsage:\n\tFulcrum.exe c prefixLen memLimit fastqFile resultFolder\n\tFulcrum.exe p prefixLen k minMismatch nVal readLen resultFolder resultFile\n");
		return 3;
	}

// pass control and exit
	int result = 0;

	if(fFastq != NULL) {
		std::clock_t timer = std::clock();
		result = cutFastq(l, memoryLimit, fFastq, resultFolder, readLen);
		fclose(fFastq);
		if(result == 0)
			printf("Cutting done. Elapsed time: %.2lf ms\n", (double)(std::clock()-timer)*1000.0/(double)CLOCKS_PER_SEC);
		else
			printf("An error occurred.\n");
	}
	if(fResult != NULL) {
		std::clock_t timer = std::clock();
		if(result==0)
			result = mergeFiles(l, k, m, n, readLen, resultFolder, fResult);
		fclose(fResult);
		if(result == 0)
			printf("Merging done. Elapsed time: %.2lf ms\n", (double)(std::clock()-timer)*1000.0/(double)CLOCKS_PER_SEC);
		else
			printf("An error occurred.\n");
	} else printf("Read length: %u\n", readLen);

	return 0;
}