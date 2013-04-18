#include "CStripeGraph.h"
#include <Windows.h>

const char* sets[] = { "eu-2005", "indochina-2004", "uk-2002", "arabic-2005" };
const int setsAmount = 1;
const int testAmount = 4;

int main(int argc, char* argv[]) {

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);

	double elapsedTime;
	int *rows, rowNr, edges, *outArr;
	char graphFileName[200], offsetsFileName[200], setFileName[200], testFileName[200];

	for(int testSet=0; testSet<setsAmount; testSet++) {

		sprintf(testFileName, "%s\\%s-test100k.txt", "D:\\webgraph\\sets", sets[testSet]);
		FILE *f = fopen(testFileName, "r");

		fscanf(f, "%d", &rowNr);
		rows = new int[rowNr];
		for(int i=0; i<rowNr; i++)
			fscanf(f, "%d", &rows[i]);
		fclose(f);
	
		CStripeGraph graph;
		
		for(int ts = 2048; ts >= 512; ts >>= 1) { // oryg 2048 - 128
			for(int ss = 8; ss <= 128; ss <<= 1) { // oryg 8 - 128

				sprintf(setFileName, "D:\\webgraph\\sets\\%s.txt", sets[testSet]);
//				printf("%s (%d,%d): ", setFileName, ts, ss);
				QueryPerformanceCounter(&t1);
//				graph.loadFromTextFile(setFileName, ts, ss);
				QueryPerformanceCounter(&t2);
				sprintf(graphFileName, "D:\\webgraph\\stripes\\%d\\%d\\%s.g", ts, ss, sets[testSet]);
				sprintf(offsetsFileName, "D:\\webgraph\\stripes\\%d\\%d\\%s.o", ts, ss, sets[testSet]);
//				graph.saveToFile(graphFileName, offsetsFileName);
//				printf(" %.4lf s\n", (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart);
				graph.loadFromFile(graphFileName, offsetsFileName, ts, ss);

				edges = 0;
				QueryPerformanceCounter(&t1);

				for(int t=0; t<testAmount; t++)
					for(int i=0; i<rowNr; i++)
						edges += graph.successorArray(rows[i], &outArr);
				
				QueryPerformanceCounter(&t2);

				elapsedTime = ((double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart) / testAmount;
				edges /= testAmount;
//				printf(" us per list: %.4lf\n ns per edge: %.4lf\n", elapsedTime * 1000000.0 / rowNr, elapsedTime * 1000000000.0 / edges);
				printf("%.4lf;%.4lf;%d;;", elapsedTime * 1000000.0 / rowNr, elapsedTime * 1000000000.0 / edges, graph.size());

				edges = 0;
				QueryPerformanceCounter(&t1);
	
				for(int t=0; t<testAmount; t++)
					for(int i=0; i<rowNr; i++)
						edges += graph.predecessorArray(rows[i], &outArr);
	
				QueryPerformanceCounter(&t2);

				elapsedTime = ((double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart) / testAmount;
				edges /= testAmount;
//				printf(" us per list: %.4lf\n ns per edge: %.4lf\n", elapsedTime * 1000000.0 / rowNr, elapsedTime * 1000000000.0 / edges);
				printf("%.4lf;%.4lf;%d;\n", elapsedTime * 1000000.0 / rowNr, elapsedTime * 1000000000.0 / edges, graph.size());

			}
		}

		delete[] rows;
	}

	return 0;
}