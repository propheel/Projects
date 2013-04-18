#include "CCharBuffer.h"

const unsigned int m[] = { 4, 8, 16, 32, 1024 };
const unsigned int mLen = 5; // 5 elements in m
const unsigned int tests = 100;

void GeneratePatterns(CCharBuffer&, CCharBuffer*);
int lab1(char* folderPath);
int lab2(char* folderPath);
int lab3(char* folderPath);
int lab4(char* folderPath);
