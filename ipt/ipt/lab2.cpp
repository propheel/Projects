#include <cstdio>
#include <string>
#include <time.h>
#include <Windows.h>
#include "definitions.h"
#include "CCharBuffer.h"
#include "TextSearch.h"

void lab2check(char* folderPath, char* testName) {

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

	const unsigned int cc = tests, bc = 3*tests, ac = mLen*bc, kt[4] = { 1, 1, 3, 1 }, taskMask[4] = { 1, 0, 0, 0 };

	for(unsigned int i=0; i<mLen; i++) {
		printf("        length:\t%u\n", m[i]);

		// naive search
		printf("          brute force:");
		for(unsigned int k=0; k<4; k++) if((taskMask[k] != 0) &&((k!=3) || (i<3))) {
			QueryPerformanceCounter(&t1);
			for(unsigned int l=0; l<kt[k]; l++)
				for(unsigned int j=0; j<tests; j++)
					naiveSearch(text, patterns[k*ac+i*bc+l*cc+j]);
			QueryPerformanceCounter(&t2);
			printf("\t%.2lf ms", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
		}
		printf("\n");

		// BMH search
		printf("          BM-Horspool:");
		for(unsigned int k=0; k<4; k++) if((taskMask[k] != 0) &&((k!=3) || (i<3))) {
			QueryPerformanceCounter(&t1);
			for(unsigned int l=0; l<kt[k]; l++)
				for(unsigned int j=0; j<tests; j++)
					HorspoolSearch(text, patterns[k*ac+i*bc+l*cc+j]);
			QueryPerformanceCounter(&t2);
			printf("\t%.2lf ms", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
		}
		printf("\n");

		if(m[i] <= 32) {
		// SO search
			printf("          ShiftOr:");
			for(unsigned int k=0; k<4; k++) if((taskMask[k] != 0) &&((k!=3) || (i<3))) {
				QueryPerformanceCounter(&t1);
				for(unsigned int l=0; l<kt[k]; l++)
					for(unsigned int j=0; j<tests; j++)
						shiftOrSearch(text, patterns[k*ac+i*bc+l*cc+j]);
				QueryPerformanceCounter(&t2);
				printf("\t%.2lf ms", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
			}
			printf("\n");

			// perform multisearch
			if(m[i] >= 16) {
				for(unsigned int r=20; r<=50; r+=30) {
					printf("          ShiftOrMulti(%u):", r);
					for(unsigned int k=0; k<4; k++) if(taskMask[k] != 0) {
						QueryPerformanceCounter(&t1);
						for(unsigned int l=0; l<kt[k]; l++)
							for(unsigned int j=0; j*r<tests; j++)
								 shiftOrMultiSearch(text, patterns+k*ac+i*bc+l*cc+j*r, r);
						QueryPerformanceCounter(&t2);
						printf("\t%.2lf ms", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
					}
					printf("\n");
				}
			}
		}
	}

	// verify ShiftOrMulti for l=32 r=10
	unsigned int l = 32, r = 10, i, matches, verifications;
	if(tests < r) r = tests;
	printf("          Verify ShiftOrMulti(%u): ", r);
	for(i=0; m[i]!=32; i++);
	printf("%.2lf%% \t", (double)shiftOrMultiSearch(text, patterns+i*bc, r, true, &matches, &verifications)/100.0);
	printf("( %u / %u )\n", matches, verifications);
}

int lab2(char* folderPath) {
	
	srand( (unsigned int)time(NULL) );

	printf("\n Lab 2:\n");
	printf(" ------\n\n");

	lab2check(folderPath, "english.processed.100MB");
	lab2check(folderPath, "dna.100MB");
	lab2check(folderPath, "proteins.100MB");

	return 0;
}