#include "definitions.h"
#include "LMGraph.h"
#include <Windows.h>

// create compressed files
void compress(int h, char *inputFileName, char *outputFilesNamesPrefix) {

	std::string resNameG, resNameO;
	resNameG.assign(outputFilesNamesPrefix).append(".g");
	resNameO.assign(outputFilesNamesPrefix).append(".o");
	printf("%s (%d) ", inputFileName, h);

	LMGraph graph;
	graph.loadFromTextFile(inputFileName, h);
	graph.saveToFiles(resNameG.c_str(), resNameO.c_str());
	graph.unLoad();

}

// test successors
void testSucc(int h, char *inputFilesNamesPrefix, char *inputTestsFileName) {

	LMGraph graph;
	LARGE_INTEGER frequency, t1, t2;
	QueryPerformanceFrequency(&frequency);

	std::string resNameG, resNameO;
	resNameG.assign(inputFilesNamesPrefix).append(".g");
	resNameO.assign(inputFilesNamesPrefix).append(".o");

	int *rows, numRows;
	FILE *f = fopen(inputTestsFileName, "r");
	fscanf(f, "%d", &numRows);
	rows = new int[numRows];
	for(int i=0; i<numRows; i++)
		fscanf(f, "%d", rows+i);
	fclose(f);

	graph.loadFromFiles(resNameG.c_str(), resNameO.c_str(), h);

	double elapsedTime = 0;
	int *sucArr, len;
	long long totalNumEdges = 0;
//	for(int i=0; i<862664; i++) {
		graph.successorArray(862663, &sucArr, &len);
		delete[] sucArr;
		totalNumEdges += len;
//	}
/*	for(int tests=0; tests<TESTS_COUNT; tests++) {

		totalNumEdges = 0;

		QueryPerformanceCounter(&t1);

		for(long long i=0; i<numRows; i++) {
			graph.successorArray(rows[i], &sucArr, &len);
			delete[] sucArr;
			totalNumEdges += len;
		}

		QueryPerformanceCounter(&t2);

		elapsedTime += (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;

	}*/
	delete[] rows;

	elapsedTime /= TESTS_COUNT;
//	printf(" : %.9lf s\n", elapsedTime);
	printf(" %lld\n", totalNumEdges);
	double usPerList = elapsedTime / numRows * 1000000.0;
	double nsPerEdge = elapsedTime / totalNumEdges * 1000000000.0;

	printf("%.3lf;%.3lf;%u", usPerList, nsPerEdge, graph.getSize());

	graph.unLoad();

}



int main(int argc, char* argv[]) {

	if(argc != 5) {
		printf("Usage: LM nodesPerChunk [ c inputFile outputFilesPrefix ] | [ t inputFilesPrefix inputTestsFile ]\n");
		return 1;
	}

	if(strcmp(argv[2], "c") == 0)
		compress(atoi(argv[1]), argv[3], argv[4]);
	else
		testSucc(atoi(argv[1]), argv[3], argv[4]);

	return 0;
}