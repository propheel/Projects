#include <map>
#include <algorithm>
#include <vector>

#define PRECISE_TIMING

#ifdef PRECISE_TIMING
#include <windows.h>
#include <mmsystem.h>
#endif

typedef unsigned int uint;

struct MyStr {
	char c[27];
	bool operator<(MyStr const& b) const {
		return std::strcmp(c, b.c) < 0;
	}
};

int main(int argc, char* argv[]) {
	if(argc != 3) {
		printf("\n\tUsage: ErdosConverter.exe inFile outFile\n\n");
		printf("Convert graph stored in inFile from Erdos Webgraph (web-graph.org) format to integer adjacency list and save it to outFile.\n");
		return 3;
	}
	std::map<MyStr, int> BlahBlah;
	std::vector<std::pair<uint,uint> > adj;
	FILE *f;
	if(fopen_s(&f, argv[1], "r") != 0) {
/* UI */		printf("Cannot open file %s.\n", argv[1]);
		return 1;
	}
	MyStr id1, id2;
	uint i1, i2;
/* UI */	printf("Reading Erdos Webgraph from %s... ", argv[1]);

#ifdef PRECISE_TIMING
	TIMECAPS resolution;
	DWORD start, finish;
	bool timingWorks = false;
	if (timeGetDevCaps (&resolution, sizeof (TIMECAPS)) == TIMERR_NOERROR)
		if (resolution.wPeriodMin <= 1)
			if (timeBeginPeriod (1) == TIMERR_NOERROR)
				timingWorks = true;
	if(timingWorks)
		start = timeGetTime();
#endif

	fscanf_s(f, "%s%s", &id1.c, sizeof(id1.c)/sizeof(char), &id2.c, sizeof(id2.c)/sizeof(char));
	while(!feof(f)) {
		if(BlahBlah.find(id1) != BlahBlah.end())
			i1 = BlahBlah[id1];
		else {
			i1 = BlahBlah.size();
			BlahBlah.insert(std::pair<MyStr,int>(id1,BlahBlah.size()));
		}

		if(BlahBlah.find(id2) != BlahBlah.end())
			i2 = BlahBlah[id2];
		else {
			i2 = BlahBlah.size();
			BlahBlah.insert(std::pair<MyStr,int>(id2,BlahBlah.size()));
		}

		adj.push_back(std::pair<uint,uint>(i1,i2));

		fscanf_s(f, "%s%s", &id1.c, sizeof(id1.c)/sizeof(char), &id2.c, sizeof(id2.c)/sizeof(char));
	}
	
#ifdef PRECISE_TIMING
	if(timingWorks) {
		finish = timeGetTime();
		timeEndPeriod(1);
/* UI */		printf("[ %d ms ]", finish-start);
	}
#endif
	
	fclose(f);
/* UI */	printf("\n%d nodes and %d edges processed.\nSorting adjacency list... ", BlahBlah.size(), adj.size());

#ifdef PRECISE_TIMING
	if(timingWorks)
		if (timeBeginPeriod (1) != TIMERR_NOERROR)
			timingWorks = false;
	if(timingWorks)
		start = timeGetTime();
#endif

	std::sort(adj.begin(), adj.end());

#ifdef PRECISE_TIMING
	if(timingWorks) {
		finish = timeGetTime();
		timeEndPeriod(1);
/* UI */		printf("[ %d ms ]", finish-start);
	}
#endif
	
/* UI */	printf("\nWriting to file %s... ", argv[2]);
	if(fopen_s(&f, argv[2], "wb") != 0) {
/* UI */		printf("Cannot open file %s.\n", argv[2]);
		return 2;
	}
	std::vector<std::pair<uint,uint> >::iterator it = adj.begin();
	i2 = -1;
	bool flag = true;

#ifdef PRECISE_TIMING
	if(timingWorks)
		if (timeBeginPeriod (1) != TIMERR_NOERROR)
			timingWorks = false;
	if(timingWorks)
		start = timeGetTime();
#endif

	for(i1 = 0; i1 < BlahBlah.size(); i1++) {
		while(flag) {
			if(it->first == i1) {
				if(i2 == i1)
					fprintf(f, " %d", it->second);
				else {
					fprintf(f, "%d", it->second);
					i2 = i1;
				}
				it++;
				flag = (it != adj.end());
			} else flag = false;
		}
		flag = (it != adj.end());
		fprintf(f, "%c", 10);
	}

#ifdef PRECISE_TIMING
	if(timingWorks) {
		finish = timeGetTime();
		timeEndPeriod(1);
/* UI */		printf("[ %d ms ]", finish-start);
	}
#endif

	fclose(f);
/* UI */	printf("\nJob's done!\n");
	return 0;
}