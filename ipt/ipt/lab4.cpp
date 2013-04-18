#include <cstdio>
#include <string>
#include <time.h>
#include <Windows.h>
#include "definitions.h"
#include "CCharBuffer.h"
#include "TextSearch.h"
#include "CSuffixArray.h"

void lab4check(char* folderPath, char* testName) {

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

	CSuffixArray sArr(text);

	const unsigned int cc = tests, bc = 3*tests, ac = mLen*bc, kt[4] = { 1, 1, 3, 1 }, taskMask[4] = { 1, 0, 0, 0 };

	for(unsigned int i=0; i<mLen; i++) {
		printf("        length:\t%u\n", m[i]);

		printf("          suffix tree:");
		for(unsigned int k=0; k<4; k++) if((taskMask[k] != 0) &&((k!=3) || (i<3))) {
			QueryPerformanceCounter(&t1);
			for(unsigned int l=0; l<kt[k]; l++)
				for(unsigned int j=0; j<tests; j++)
					binarySearch(sArr, patterns[k*ac+i*bc+l*cc+j]);
			QueryPerformanceCounter(&t2);
			printf("\t%.2lf ms", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
		}
		printf("\n");

	}
	printf("\n");
}

int lab4(char* folderPath) {
	
	srand( (unsigned int)time(NULL) );

	printf("\n Lab 4:\n");
	printf(" ------\n\n");

	lab4check(folderPath, "english.processed.100MB");
	lab4check(folderPath, "dna.100MB");

	return 0;
}