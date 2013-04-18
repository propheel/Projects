#ifndef _CSUFFIXARRAY_
#define _CSUFFIXARRAY_

#include "CCharBuffer.h"
#include <vector>

class CSuffixArray {
public:
	struct myStr {
		char *v;
		unsigned long l;
		myStr(char* values, unsigned long length) : v(values), l(length) {}
		bool operator<(const myStr &p) const;
	};

	CSuffixArray(CCharBuffer &text);
	unsigned int size() const;
	char *getSuffix(unsigned int ind, unsigned int *len) const;
	myStr getSuffix(unsigned int ind) const;
private:
	std::vector<myStr> arr;
};

#endif