#define OS_WIN /* We're using MSVC. Another options *will* be added :) */
#define _CRT_SECURE_NO_WARNINGS
#define _FILE_OFFSET_BITS 64
#include <cstdio>
#include <string>

const int readChunk = 32768000;
const int residBuffer = 32768;
const int chunksPerZip = 4;
