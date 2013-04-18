#include "definitions.h"
#include "LMGraph.h"
#include <Windows.h>
#include <string>

#define TESTS_COUNT 10

const char * setsDir = "D:\\webgraph\\sets\\";
const char * sets[] = { "eu-2005", "eu-2005t", "indochina-2004", "indochina-2004t", "uk-2002", "uk-2002t", "arabic-2005", "arabic-2005t" };
const int setsAmount = 8;
const char * resDir = "D:\\webgraph\\LM2\\";
const char * compressLog = "D:\\webgraph\\results\\compressLogLM2.csv";
const char * decompressLog = "D:\\webgraph\\results\\decompressLogLM2.csv";
const char * testLog = "D:\\webgraph\\results\\testLogLM2.csv";

// create compressed files and decompress them
void compressAndDecompress() {

	LMGraph graph;
	std::string fNameT, resNameG, resNameO;
	char tmpBuf[10];

	FILE *f = fopen(compressLog, "w");
	fprintf(f, ";;");
	for(int i=0; i<setsAmount; i++)
		fprintf(f, ";%s", sets[i]);
	fprintf(f, "\n");

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);

	// compress with zip
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(f, "zip;8");
		else
			fprintf(f, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			fNameT.assign(setsDir).append(sets[setNr]).append(".txt");
			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".g");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".o");
			printf("%s (%d,zip)", fNameT.c_str(), h);

			QueryPerformanceCounter(&t1);

			graph.loadFromTextFile(fNameT.c_str(), h, true);
			graph.saveToFile(resNameG.c_str(), resNameO.c_str());

			QueryPerformanceCounter(&t2);

			double elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
			fprintf(f, ";%.9lf", elapsedTime);
			printf(" : %.9lf\n", elapsedTime);
			//graph.printStats();
			
		}

		fprintf(f, "\n");
	}
/*	
	// compress with nopack
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(f, "nopack;8");
		else
			fprintf(f, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			fNameT.assign(setsDir).append(sets[setNr]).append(".txt");
			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".g");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".o");
			printf("%s (%d,nopack)", fNameT.c_str(), h);

			QueryPerformanceCounter(&t1);

			graph.loadFromTextFile(fNameT.c_str(), h, false);
			graph.saveToFile(resNameG.c_str(), resNameO.c_str());

			QueryPerformanceCounter(&t2);

			double elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
			fprintf(f, ";%.9lf", elapsedTime);
			printf(" : %.9lf\n", elapsedTime);
			
		}

		fprintf(f, "\n");
	}
	*/
	// open log
	fclose(f);
/*	f = fopen(decompressLog, "w");
	fprintf(f, ";");
	for(int i=0; i<setsAmount; i++)
		fprintf(f, ";%s", sets[i]);
	fprintf(f, "\n");

	// decompress with zip
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(f, "zip;8");
		else
			fprintf(f, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".g");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".o");
			fNameT.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append(sets[setNr]).append(".z.txt");
			printf("%s (%d,zip)", fNameT.c_str(), h);

			QueryPerformanceCounter(&t1);

			graph.loadFromFile(resNameG.c_str(), resNameO.c_str(), h, true);
			graph.saveToTextFile(fNameT.c_str());

			QueryPerformanceCounter(&t2);

			double elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
			fprintf(f, ";%.9lf", elapsedTime);
			printf(" : %.9lf\n", elapsedTime);
			
		}

		fprintf(f, "\n");
	}
	
	// decompress with nopack
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(f, "nopack;8");
		else
			fprintf(f, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("graph.b");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("offsets.b");
			fNameT.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append(sets[setNr]).append(".b.txt");
			printf("%s (%d,nopack)", fNameT.c_str(), h);

			QueryPerformanceCounter(&t1);

			graph.loadFromFile(resNameG.c_str(), resNameO.c_str(), h, false);
			graph.saveToTextFile(fNameT.c_str());

			QueryPerformanceCounter(&t2);

			double elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
			fprintf(f, ";%.9lf", elapsedTime);
			printf(" : %.9lf\n", elapsedTime);
			
		}

		fprintf(f, "\n");
	}
	
	fclose(f);*/
}



void testSucc() {

	LMGraph graph;
	std::string fNameT, resNameG, resNameO;
	char tmpBuf[10];

	FILE *flog = fopen(testLog, "w");
	fprintf(flog, ";;");
	for(int i=0; i<setsAmount; i++)
		fprintf(flog, ";%s;;;", sets[i]);
	fprintf(flog, "\n");

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);

	// test with zip
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(flog, "zip;8");
		else
			fprintf(flog, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			fNameT.assign(setsDir).append(sets[setNr]).append("-test100k.txt");
			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".g");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append(".o");
			printf("%s (%d,zip)", fNameT.c_str(), h);

			FILE *f = fopen(fNameT.c_str(), "r");
			long long *row, ile;
			fscanf(f, "%lld", &ile);
			row = new long long[ile];
			for(long long i=0; i<ile; i++)
				fscanf(f, "%lld", row+i);
			fclose(f);

			graph.loadFromFile(resNameG.c_str(), resNameO.c_str(), h, true);
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
/*	
	// test with nopack
	for(int h=8; h<=128; h<<=1) {

		if(h==8)
			fprintf(flog, "nopack;8");
		else
			fprintf(flog, ";%d", h);

		for(int setNr=0; setNr<setsAmount; setNr++) {

			fNameT.assign(setsDir).append(sets[setNr]).append("-test100k.txt");
			resNameG.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("graph.b");
			resNameO.assign(resDir).append(itoa(h, tmpBuf, 10)).append("\\").append(sets[setNr]).append("\\").append("offsets.b");
			printf("%s (%d,nopack)", fNameT.c_str(), h);

			FILE *f = fopen(fNameT.c_str(), "r");
			long long *row, ile;
			fscanf(f, "%lld", &ile);
			row = new long long[ile];
			for(long long i=0; i<ile; i++)
				fscanf(f, "%lld", row+i);
			fclose(f);

			graph.loadFromFile(resNameG.c_str(), resNameO.c_str(), h, false);
			double elapsedTime = 0;
			long long *sucArr, len, edges = 0;
			sucArr = new long long[graph.getMaxResidues()];
			for(int tests=0; tests<TESTS_COUNT; tests++) {

				edges = 0;

				QueryPerformanceCounter(&t1);

				for(long long i=0; i<ile; i++) {
					graph.successorArray(row[i], sucArr, len);
					edges += len;
				}
	
				QueryPerformanceCounter(&t2);

				elapsedTime += (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
			}

			delete[] sucArr;
			elapsedTime /= TESTS_COUNT;
			printf(" : %.9lf s\n", elapsedTime);
			double usPerList = elapsedTime / ile * 1000000.0;
			double nsPerEdge = elapsedTime / edges * 1000000000.0;
			printf("\t%.3lf us, %.3lf ns, %lld edges\n", usPerList, nsPerEdge, edges);

			fprintf(flog, ";%.3lf;%.3lf;%lld;", usPerList, nsPerEdge, graph.getLength());
		}
		fprintf(flog, "\n");
	}
*/
	fclose(flog);
}



int main(int argc, char* argv[]) {

//	compressAndDecompress();
	testSucc();

	return 0;
}