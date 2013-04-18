#include "DSGraph.h"

int main(int argc, char* argv[]) {
	DSGraph graph;
	graph.loadFromTextFile("D:\\webgraph\\sets\\indochina-2004.txt");
	graph.saveToFile("D:\\webgraph\\DiagSquares\\indochina-2004.g", "D:\\webgraph\\DiagSquares\\indochina-2004.o");
/*	graph.loadFromFile("D:\\webgraph\\DiagSquares\\eu-2005.g", "D:\\webgraph\\DiagSquares\\eu-2005.o");

	int *sucArr;
	unsigned int edges = 0;
	for(int i=0; i<1000; i++)
		edges += graph.successorArray(i, &sucArr);

	printf("%u\n", edges);
	*/
	return 0;
}