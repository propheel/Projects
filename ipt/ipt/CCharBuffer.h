#ifndef _CCHARBUFFER_
#define _CCHARBUFFER_

class CCharBuffer {
	char *buffer;
	unsigned long size, *refCounter;
public:
	CCharBuffer();
	CCharBuffer(const CCharBuffer &p);
	CCharBuffer(const char *fileName);
	~CCharBuffer();
	char& operator[](int index);
	const char& operator[](int index) const;
	unsigned int length();
	bool readFromFile(const char *fileName);
	void toLower();
	void remSpaces();
	void addSpaceForDot();
	bool writeToFile(const char *fileName, const char *mode);
	bool copyFrom(CCharBuffer const &c, unsigned int pos=0, unsigned int len=0);
	bool copyFrom(const char *string);
	bool reverse();
	bool incLevenshtein();
	bool hasSpaces(unsigned int pos, unsigned int len);
	bool operator()(const CCharBuffer &p) const;
	char *getBuffer();
};

#endif