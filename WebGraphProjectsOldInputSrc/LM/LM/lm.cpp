#include "definitions.h"
#include "LMGraph.h"
#include <Windows.h>
#include <string>

#define TESTS_COUNT 10

const char * setsDir = "D:\\webgraph\\sets\\"; // where to find *.txt
const char * sets[] = { "eu-2005", "eu-2005t", "indochina-2004", "indochina-2004t", "uk-2002", "uk-2002t", "arabic-2005", "arabic-2005t", "ljournal-2008", "ljournal-2008t", "uk-2007-05", "uk-2007-05t" }; // names of sets
const int setsAmount = 1; // how many sets to process
const char * resDir = "D:\\webgraph\\LM\\cpp\\"; // where to put compressed sets
const char * compressLog = "D:\\webgraph\\results\\compressLogLM.csv"; // compression log file
const char * testLog = "D:\\webgraph\\results\\testLogLM.csv"; // test log file

LMGraph graph;
std::string fNameT, resNameG, resNameO;
char tmpBuf[10];
LARGE_INTEGER frequency;
LARGE_INTEGER t1, t2;

// create compressed files
void compress() {

	// open compression log
	FILE *f = fopen(compressLog, "w");
	fprintf(f, ";");
	for(int i=0; i<setsAmount; i++)
		fprintf(f, ";%s", sets[i]);
	fprintf(f, "\n");

	// compress with zip
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(f, "LM;8");
		else
			fprintf(f, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			fNameT.assign(setsDir).append(sets[setNr]).append(".txt");
			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("graph.z");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("offsets.z");
			printf("%s (%d) ", fNameT.c_str(), h);

			QueryPerformanceCounter(&t1);

			graph.loadFromTextFile(fNameT.c_str(), h);
			graph.saveToFile(resNameG.c_str(), resNameO.c_str());

			QueryPerformanceCounter(&t2);

			double elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
			fprintf(f, ";%.9lf", elapsedTime);
			printf(" : %.9lf\n", elapsedTime);
			//graph.printStats();
			
		}
		fprintf(f, "\n");
	}
	fclose(f);
}



void testSucc() {

	// open test log
	FILE *flog = fopen(testLog, "w");
	fprintf(flog, ";");
	for(int i=0; i<setsAmount; i++)
		fprintf(flog, ";%s;;;", sets[i]);
	fprintf(flog, "\n");

	// test with zip
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(flog, "LM;8");
		else
			fprintf(flog, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			fNameT.assign(setsDir).append(sets[setNr]).append("-test100k.txt");
			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("graph.z");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("offsets.z");
			printf("%s (%d) ", fNameT.c_str(), h);

			FILE *f = fopen(fNameT.c_str(), "r");
			long long *row, ile;
			fscanf(f, "%lld", &ile);
			row = new long long[ile];
			for(long long i=0; i<ile; i++)
				fscanf(f, "%lld", row+i);
			fclose(f);

			graph.loadFromFile(resNameG.c_str(), resNameO.c_str(), h);
			double elapsedTime = 0;
			long long *sucArr, len, edges = 0;
			for(int tests=0; tests<TESTS_COUNT; tests++) {

				edges = 0;

				QueryPerformanceCounter(&t1);

				for(long long i=0; i<ile; i++) {
					sucArr = graph.successorArray(row[i], len);
//					printf("%lld %lld\n", row[i], len);
					delete[] sucArr;
					edges += len;
				}
	
				QueryPerformanceCounter(&t2);

				elapsedTime += (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
			}

			delete[] row;
			elapsedTime /= TESTS_COUNT;
			printf(" : %.9lf s\n", elapsedTime);
			double usPerList = elapsedTime / ile * 1000000.0;
			double nsPerEdge = elapsedTime / edges * 1000000000.0;
			printf("\t%.3lf us, %.3lf ns, %lld edges\n", usPerList, nsPerEdge, edges);

			fprintf(flog, ";%.3lf;%.3lf;%lld;", usPerList, nsPerEdge, graph.getLength());
		}
		fprintf(flog, "\n");
	}
	fclose(flog);
}



int main(int argc, char* argv[]) {

	QueryPerformanceFrequency(&frequency);

	compress();
	testSucc();

	return 0;
}