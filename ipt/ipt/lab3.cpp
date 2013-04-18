#include <cstdio>
#include <string>
#include <time.h>
#include <Windows.h>
#include "definitions.h"
#include "CCharBuffer.h"
#include "TextSearch.h"

void lab3check(char* folderPath, char* testName) {

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	std::string path(folderPath);
	std::string fileName;

	printf("     read %s\n", testName);
	CCharBuffer text(fileName.assign(path).append(testName).c_str());

	printf("     check %s:\n", testName);
	CCharBuffer patterns[4*mLen*3*tests];
	GeneratePatterns(text, patterns);

	unsigned int cc = tests, bc = 3*tests, ac = mLen*bc, kt[4] = { 1, 1, 3, 1 }, taskMask[4] = { 0, 0, 1, 0 };

	for(unsigned int i=0; i<(mLen-1); i++) {
		printf("        length:\t%u\n", m[i]);

		// dynamic programming
		printf("          dynamic programming:");
		for(unsigned int k=0; k<4; k++) if((taskMask[k] != 0) &&((k!=3) || (i<3)))
			for(unsigned int l=0; l<kt[k]; l++) {
				QueryPerformanceCounter(&t1);
				for(unsigned int j=0; j<tests; j++)
					dynamicApproximateSearch(text, patterns[k*ac+i*bc+l*cc+j], l+1);
				QueryPerformanceCounter(&t2);
				printf("\t%.2lf ms", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
			}
		printf("\n");

		// counting filter
		printf("          counting filter:");
		for(unsigned int k=0; k<4; k++) if((taskMask[k] != 0) &&((k!=3) || (i<3)))
			for(unsigned int l=0; l<kt[k]; l++) {
				QueryPerformanceCounter(&t1);
				for(unsigned int j=0; j<tests; j++)
					countingFilterApproximateSearch(text, patterns[k*ac+i*bc+l*cc+j], l+1);
				QueryPerformanceCounter(&t2);
				printf("\t%.2lf ms", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
			}
		printf("\n");
	}
	printf("\n");
}

int lab3(char* folderPath) {

	srand( (unsigned int)time(NULL) );

	printf("\n Lab 3:\n");
	printf(" ------\n\n");

	lab3check(folderPath, "english.processed.100MB");
	lab3check(folderPath, "dna.100MB");

	return 0;
}