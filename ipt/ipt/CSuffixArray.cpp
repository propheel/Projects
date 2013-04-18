#include <cstdio>
#include <string>
#include <algorithm>
#include <Windows.h>
#include "CSuffixArray.h"

bool CSuffixArray::myStr::operator<(const myStr &p) const {
	int res = strncmp(v, p.v, (l<p.l) ? l : p.l);
	if(res == -1)
		return true;
	else
		if(res == 1)
			return false;
		else
			return (l<p.l);
}

CSuffixArray::CSuffixArray(CCharBuffer &text) {

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);

	printf("            reserving memory for suffixes ");
	QueryPerformanceCounter(&t1);
	arr.reserve(text.length());
	QueryPerformanceCounter(&t2);
	printf("%.2lf ms\n", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);

	printf("            adding suffixes ");
	QueryPerformanceCounter(&t1);
	for(unsigned int i=0; i<text.length(); i++)
		arr.push_back(myStr(&text.getBuffer()[i], text.length()-i));
	QueryPerformanceCounter(&t2);
	printf("%.2lf ms\n", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);

	printf("            sorting suffixes ");
	QueryPerformanceCounter(&t1);
	std::sort(arr.begin(), arr.end());
	QueryPerformanceCounter(&t2);
	printf("%.2lf ms\n", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);

}

unsigned int CSuffixArray::size() const {	return arr.size(); }

char * CSuffixArray::getSuffix(unsigned int ind, unsigned int *len) const {
	if(len != NULL)
		*len = arr[ind].l;
	return arr[ind].v;
}

CSuffixArray::myStr CSuffixArray::getSuffix(unsigned int ind) const { return arr[ind]; }