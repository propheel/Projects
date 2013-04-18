#include <string>
#include "CCharBuffer.h"
#include "definitions.h"

void GeneratePatterns(CCharBuffer &text, CCharBuffer* patterns) {
	const unsigned int cc = tests, bc = 3*tests, ac = mLen*bc;
	for(unsigned int i=0; i<mLen; i++) {
		for(unsigned int j=0; j<tests; j++) {
			unsigned int pos = rand() % (text.length() - m[i]);
			patterns[i*bc+j].copyFrom(text, pos, m[i]);
		}
	}
	for(unsigned int i=0; i<mLen; i++) {
		for(unsigned int j=0; j<tests; j++) {
			unsigned int pos = rand() % (text.length() - m[i]);
			patterns[ac+i*bc+j].copyFrom(text, pos, m[i]);
			patterns[ac+i*bc+j].reverse();
		}
	}
	for(unsigned int i=0; i<mLen; i++) {
		for(unsigned int l=0; l<3; l++) {
			for(unsigned int j=0; j<tests; j++) {
				patterns[2*ac+i*bc+l*cc+j].copyFrom(patterns[i*bc+j]);
				for(unsigned int x=0; x<=l; x++)
					patterns[2*ac+i*bc+l*cc+j].incLevenshtein();
			}
		}
	}
	for(unsigned int i=0; i<3; i++) {
		for(unsigned int j=0; j<tests; j++) {
			unsigned int pos;
			do
				pos = rand() % (text.length() - (i+2));
			while (text.hasSpaces(pos, i+2));
			patterns[3*ac+i*bc+j].copyFrom(text, pos, i+2);
		}
	}
}