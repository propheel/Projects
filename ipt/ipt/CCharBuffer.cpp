#include <cstdio>
#include <string>
#include "CCharBuffer.h"

CCharBuffer::CCharBuffer() {
	buffer = NULL;
	size = 0;
	refCounter = new unsigned long;
	*refCounter = 1;
}

CCharBuffer::CCharBuffer(const char *fileName) {
	buffer = NULL;
	size = 0;
	refCounter = new unsigned long;
	*refCounter = 1;
	readFromFile(fileName);
}

CCharBuffer::CCharBuffer(const CCharBuffer &p) {
	buffer = p.buffer;
	size = p.size;
	refCounter = p.refCounter;
	(*refCounter)++;
}

CCharBuffer::~CCharBuffer() {
	if(((*refCounter) < 2) && (buffer!=NULL)) free(buffer);
	(*refCounter)--;
}

char& CCharBuffer::operator[](int index) {
	return buffer[index];
}

const char& CCharBuffer::operator[](int index) const {
	return buffer[index];
}

unsigned int CCharBuffer::length() { return size; }

bool CCharBuffer::readFromFile(const char *fileName) {
	if(buffer!=NULL) free(buffer);
	FILE *f = fopen(fileName, "r");
	if(f==NULL) {
		printf("Error: Cannot open file %s.\n", fileName);
		return false;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	buffer = (char*)malloc(size);
	if(buffer==NULL) {
		fclose(f);
		printf("Error: Cannot allocate memory.\n");
		return false;
	}
	fread(buffer, 1, size, f);
	fclose(f);
	return true;
}

void CCharBuffer::toLower() {
	unsigned int dotCount = 0;
	for(unsigned int i=0; i<size; i++) {
		if((buffer[i] >= 'A') && (buffer[i] <= 'Z')) {
			buffer[i] += 'a'-'A';
			dotCount = 0;
		} else {
			if(buffer[i] =='.') {
				if(dotCount==0)
					dotCount=1;
				else
					buffer[i] = ' ';
			} else if((buffer[i]<'0') || ((buffer[i]>'9') && (buffer[i]<'a')) || (buffer[i]>'z'))
					buffer[i] = ' ';
				else
					dotCount = 0;
		}
	}
}
void CCharBuffer::remSpaces() {
	unsigned int spaces = 0, offset = 0, pos = 0;
	for(unsigned int i=0; i<size; i++)
		if(buffer[i]==' ') spaces++; else {
			if(spaces > 1) {
				memmove(buffer+pos-offset, buffer+pos, i-pos-(spaces-1));
				offset += spaces-1;
				pos = i;
			}
			spaces = 0;
		}
	size -= offset;
}

void CCharBuffer::addSpaceForDot() {
	bool space = false;
	bool dot = false;
	unsigned int toAdd = 0;
	for(unsigned int i=0; i<size; i++)
		if(buffer[i] == '.') {
			if(!space)
				toAdd++;
			space = false;
			dot = true;
		} else {
			if(buffer[i] == ' ')
				space = true;
			else {
				if(dot)
					toAdd++;
				space = false;
			}
			dot = false;
		}
	char *newB = (char*)malloc(size+toAdd);
	if(newB==NULL) {
		printf("Error: Memory allocation failed.\n");
		return;
	}
	space = false;
	dot = false;
	unsigned int newI = 0;
	for(unsigned int i=0; i<size; i++) {
		if(buffer[i] == '.') {
			if(!space)
				newB[newI++] = ' ';
			space = false;
			dot = true;
		} else {
			if(buffer[i] == ' ')
				space = true;
			else {
				if(dot)
					newB[newI++] = ' ';
				space = false;
			}
			dot = false;
		}
		newB[newI++] = buffer[i];
	}
	free(buffer);
	buffer = newB;
	size = newI;
}

bool CCharBuffer::writeToFile(const char *fileName, const char *mode) {
	FILE *f = fopen(fileName, mode);
	if(f==NULL) {
		printf("Error: Cannot open file %s\n", fileName);
		return false;
	}
	if(strcmp(mode, "a")==0)
		fwrite("\n", 1, 1, f);
	fwrite(buffer, 1, size, f);
	fclose(f);
	return true;
}

bool CCharBuffer::copyFrom(CCharBuffer const &c, unsigned int pos, unsigned int len) {
	if(pos > c.size) return false;
	if(buffer!=NULL) free(buffer);
	if(len == 0) len = c.size - pos;
	size = len;
	buffer = (char*)malloc(size);
	if(buffer==NULL) {
		printf("Error: Cannot allocate memory. [size %u]\n", size);
		size = 0;
		return false;
	}
	memcpy(buffer, c.buffer+pos, size);
	return true;
}

bool CCharBuffer::copyFrom(const char *string) {
	if(buffer!=NULL) free(buffer);
	size = strlen(string);
	buffer = (char*)malloc(size);
	if(buffer==NULL) {
		printf("Error: Cannot allocate memory. [size %u]\n", size);
		size = 0;
		return false;
	}
	memcpy(buffer, string, size);
	return true;
}

bool CCharBuffer::reverse() {
	unsigned int halfSize = (size+1)>>1;
	char *tmp = (char*)malloc(halfSize);
	if(tmp==NULL) {
		printf("Error: Cannot allocate memory. [size %u]\n", halfSize);
		return false;
	}
	memcpy(tmp, buffer, halfSize);
	for(unsigned int i=halfSize; i<size; i++) {
		buffer[size-i-1] = buffer[i];
		buffer[i] = tmp[size-i-1];
	}
	free(tmp);
	return true;
}

bool CCharBuffer::incLevenshtein() {
	unsigned int opType = rand() % 3;
	unsigned int pos;
	char *tmp;
	switch(opType) {
	case 0: // insertion
		tmp = (char*)malloc(size+1);
		if(tmp==NULL) {
			printf("Error: Cannot allocate memory. [size %u]\n", size+1);
			return false;
		}
		pos = rand() % (size+1);
		memcpy(tmp, buffer, pos);
		tmp[pos] = rand() % 26 + 97;
		memcpy(tmp+pos+1, buffer+pos, size-pos);
		size++;
		free(buffer);
		buffer = tmp;
		break;
	case 1: // replacement
		pos = rand() % size;
		buffer[pos] = rand() % 26 + 97;
		break;
	case 2: // deletion
		pos = rand() % size;
		memcpy(buffer+pos, buffer+pos+1, (size-1)-pos);
		size--;
		break;
	}
	return true;
}

bool CCharBuffer::hasSpaces(unsigned int pos, unsigned int len) {
	if(pos+len > size) len = size-pos;
	while(len > 0) {
		if(buffer[pos] == ' ')
			return true;
		pos++;
		len--;
	}
	return false;
}

bool CCharBuffer::operator()(const CCharBuffer &p) const {
	for(unsigned int i=0; ((i<size) && (i<p.size)); i++)
		if(buffer[i] < p.buffer[i])
			return true;
		else if(buffer[i] > p.buffer[i])
			return false;
	return (size<p.size);
}

char *CCharBuffer::getBuffer() { return buffer; }