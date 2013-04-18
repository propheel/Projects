#include <cstdio>
#include <string>
#include <time.h>
#include <Windows.h>
#include "definitions.h"
#include "CCharBuffer.h"

int lab1(char* folderPath) {

	srand( (unsigned int)time(NULL) );

	std::string path(folderPath);
	std::string fileName;
	char *tmpBuf = (char*)malloc(10);
	if(tmpBuf==NULL) {
		printf("Error: Cannot allocate memory.\n");
		return 1;
	}

	printf("\n Lab 1:\n");
	printf(" ------\n\n");
	printf("  1. read english.100MB\n");
	CCharBuffer english(fileName.assign(path).append("english.100MB").c_str());
	printf("     convert to lower\n");
	english.toLower();
	printf("     remove spaces\n");
	english.remSpaces();
	printf("     add space for dots\n");
	english.addSpaceForDot();
	printf("     save to english.processed.100MB\n");
	english.writeToFile(fileName.assign(path).append("english.processed.100MB").c_str(), "w");

	CCharBuffer patterns[4*mLen*3*tests]; // WOOOW!
	GeneratePatterns(english, patterns);

	unsigned int cc = tests, bc = 3*tests, ac = mLen*bc;

	printf("\n  3. a) %u patterns:\n", tests);
	for(unsigned int i=0; i<mLen; i++) {
		printf("        length = %u\n", m[i]);
		fileName.assign(path).append("english.a").append(itoa(m[i], tmpBuf, 10));
		for(unsigned int j=0; j<tests; j++)
			patterns[i*bc+j].writeToFile(fileName.c_str(), (j==0) ? "w" : "a");
	}

	printf("\n  3. b) %u patterns reversed:\n", tests);
	for(unsigned int i=0; i<mLen; i++) {
		printf("        length = %u\n", m[i]);
		fileName.assign(path).append("english.b").append(itoa(m[i], tmpBuf, 10));
		for(unsigned int j=0; j<tests; j++)
			patterns[ac+i*bc+j].writeToFile(fileName.c_str(), (j==0) ? "w" : "a");
	}

	printf("\n  3. c) %u approximate patterns:\n", tests);
	for(unsigned int i=0; i<mLen; i++) {
		printf("        length = %u [ ", m[i]);
		for(unsigned int l=0; l<3; l++) {
			printf("lev%u ", l+1);
			fileName.assign(path).append("english.c").append(itoa(m[i], tmpBuf, 10));
			fileName.append("_").append(itoa(l+1, tmpBuf, 10));
			for(unsigned int j=0; j<tests; j++)
				patterns[2*ac+i*bc+l*cc+j].writeToFile(fileName.c_str(), (j==0) ? "w" : "a");
		}
		printf("]\n");
	}

	printf("\n  3. d) %u patterns without spaces:\n", tests);
	for(unsigned int i=0; i<3; i++) {
		printf("        length = %u\n", i+2);
		fileName.assign(path).append("english.d").append(itoa(i+2, tmpBuf, 10));
		for(unsigned int j=0; j<tests; j++)
			patterns[3*ac+i*bc+j].writeToFile(fileName.c_str(), (j==0) ? "w" : "a");
	}
	printf("\n");

	free(tmpBuf);

	return 0;
}