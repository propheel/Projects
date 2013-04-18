#include <string>
#include "TextSearch.h"
#include <cstdio>

int naiveSearch(CCharBuffer &text, CCharBuffer &pattern) {
	unsigned int n = text.length(), m = pattern.length();
	int res = -1;
	for(unsigned int i=0; i<n-m; i++) {
		res = (int)i;
		for(unsigned int j=0; j<m; j++)
			if(text[i+j] != pattern[j]) {
				res = -1;
				break;
			}
		if(res!=-1) break;
	}
	return res;
}

int HorspoolSearch(CCharBuffer &text, CCharBuffer &pattern) {
	unsigned int n = text.length(), m = pattern.length();
	unsigned int d[256];
	for(unsigned int i=0; i<256; i++) d[i] = m;
	unsigned int last = m-1;
	for(unsigned int j=0; j<last; j++)
		d[pattern[j]] = last-j;
	for(unsigned int i=0; i+last<n; i+=d[text[i+last]]) {
		while((i+last<n) && (text[i+last]!=pattern[last])) i+=d[text[i+last]];
		bool match = true;
		for(unsigned int j=0; j<last; j++)
			if(pattern[j] != text[i+j]) {
				match = false;
				break;
			}
		if(match) return (int)i;
	}
	return -1;
}

int shiftOrSearch(CCharBuffer &text, CCharBuffer &pattern) {
	unsigned int n = text.length(), m = pattern.length();
	unsigned int b[256];
	for(unsigned int i=0; i<256; i++) b[i] = ~0;
	for(unsigned int i=0; i<m; i++) b[pattern[i]] = b[pattern[i]] & ~(1 << i);
	unsigned int d = ~0;
	for(unsigned int i=0; i<n; i++) {
		d = (d<<1) | b[text[i]];
		if((d&(1<<(m-1)))==0) return i-m+1;
	}
	return -1;
}

int shiftOrMultiSearch(CCharBuffer &text, CCharBuffer *patterns, unsigned int count, bool verify, unsigned int *matches, unsigned int *verifications) {
	if(verify) (*matches) = (*verifications) = 0;
	unsigned int n = text.length(), m = 0;
	unsigned int b[256];
	for(unsigned int i=0; i<256; i++) b[i] = ~0;
	for(unsigned int i=0; i<count; i++) {
		for(unsigned int j=0; j<patterns[i].length(); j++)
			b[patterns[i][j]] = b[patterns[i][j]] & ~(1 << j);
		if(patterns[i].length() > m) m = patterns[i].length();
	}
	unsigned int d = ~0;
	for(unsigned int i=0; i<n; i++) {
		d = (d<<1) | b[text[i]];
		if((d&(1<<(m-1)))==0)
			if(verify) {
				(*verifications)++;
				for(unsigned int j=0; j<count; j++) {
					bool match = true;
					for(unsigned int k=0; k<patterns[j].length(); k++)
						if(text[i-patterns[j].length()+1+k] != patterns[j][k]) {
							match = false;
							break;
						}
					if(match) {
						(*matches)++;
						break;
					}
				}
			} else return i-m+1;
	}
	if(verify)
		return (*matches)*10000/(*verifications);
	else
		return -1;
}

unsigned int min(unsigned int a, unsigned int b, unsigned int c) {
	if(a<b)
		if(a<c)
			return a;
		else
			return c;
	else
		if(b<c)
			return b;
		else
			return c;
}

unsigned int t[34];

int dynamicApproximateSearch(CCharBuffer &text, CCharBuffer &pattern, unsigned int k, unsigned int start, unsigned int len) {
	unsigned int n = text.length(), m = pattern.length();
	if(len!=0)
		n = len;
	for(unsigned int i=0; i<=m; i++) t[i] = i;
	unsigned int l, v, p;
	for(unsigned int i=0; i<n; i++) {
		l=0;
		for(unsigned int j=0; j<m; j++) {
			if(text[start+i]==pattern[j])
				p=0;
			else
				p=1;
			v = min(t[j+1]+1, l+1, t[j]+p);
			t[j] = l;
			l = v;
		}
		t[m] = l;
		if(l <= k)
			return i;
	}
	return -1;
}

int countingFilterApproximateSearch(CCharBuffer &text, CCharBuffer &pattern, unsigned int k) {
	unsigned int n = text.length(), m = pattern.length();
	int count = -(int)(m-k), a[256];
	for(unsigned int c=0; c<256; c++) a[c] = 0;
	for(unsigned int j=0; j<m; j++) a[(unsigned char)pattern[j]]++;
	for(unsigned int i=0; i<m; i++)
		if(a[(unsigned char)text[i]]-- > 0)
			count++;
	for(unsigned int i=m; i<=n; i++) {
		if(count >= 0) {
			int res = dynamicApproximateSearch(text, pattern, k, i-m, m+k);
			if(res!=-1)
				return i-m ;
		}
		if(++a[(unsigned char)text[i-m]] > 0) count--;
		if(a[(unsigned char)text[i]]-- > 0) count++;
	}
	return -1;
}

int binarySearch(const CSuffixArray &sArray, CCharBuffer &pattern) {
	int l=0, r=sArray.size()-1, pos = sArray.size()/2;
	CSuffixArray::myStr pat(pattern.getBuffer(), pattern.length());
	while(r-l > 2) {
		if(pat < sArray.getSuffix(pos))
			r = pos-1;
		else
			l = pos;
		pos = (l+r)/2;
	}
	if(pat < sArray.getSuffix(l))
		if(strncmp(pat.v, sArray.getSuffix(l).v, pat.l) == 0)
			return l;
		else
			return -1;
	else
		if(sArray.getSuffix(l) < pat)
			if(pat < sArray.getSuffix(r))
				if(strncmp(pat.v, sArray.getSuffix(r).v, pat.l) == 0)
					return r;
				else
					return -1;
			else
				if(sArray.getSuffix(r) < pat)
					return -1;
				else
					return r;
		else
			return l;
}