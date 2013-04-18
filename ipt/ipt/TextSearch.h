#include "CCharBuffer.h"
#include "CSuffixArray.h"

int naiveSearch(CCharBuffer &text, CCharBuffer &pattern);
int HorspoolSearch(CCharBuffer &text, CCharBuffer &pattern);
int shiftOrSearch(CCharBuffer &text, CCharBuffer &pattern);
int shiftOrMultiSearch(CCharBuffer &text, CCharBuffer *patterns, unsigned int count, bool verify=false, unsigned int *matches=0, unsigned int *verifications=0);
int dynamicApproximateSearch(CCharBuffer &text, CCharBuffer &pattern, unsigned int k, unsigned int start=0, unsigned int len=0);
int countingFilterApproximateSearch(CCharBuffer &text, CCharBuffer &pattern, unsigned int k);
int binarySearch(const CSuffixArray &sArray, CCharBuffer &pattern);