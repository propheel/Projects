#define CHARS_READ 1000 /* how many chars from the beggining of the file to read */

#include <Windows.h>
#include <CLucene.h>
#include "boost/filesystem.hpp"

using namespace lucene::analysis;
using namespace lucene::index;
using namespace lucene::document;
using namespace lucene::queryParser;
using namespace lucene::search;
using namespace lucene::store;

namespace bfs = boost::filesystem3;

const char * INDEX_DIR = "index.dat";
#define QUERY "the time"
const bool BUILD_INDEX = true;


unsigned long long buildIndex(const char *indexDir, const char *txtFilesDir) {

	unsigned char* buffer = new unsigned char[CHARS_READ];

	SimpleAnalyzer analyzer;

	unsigned long long filesIndexed = 0;

	try {
		Directory *dir = FSDirectory::getDirectory(indexDir);
		IndexWriter *writer = new IndexWriter(dir, &analyzer, true);
		Document doc;

		for(bfs::directory_iterator inDir(txtFilesDir); inDir!=bfs::directory_iterator(); inDir++) {

			// read file
			FILE *fin = fopen(inDir->path().string().c_str(), "rb");
			if(fin == NULL) { printf("Error reading file.\n"); return 0; }
			unsigned long long fSize;
			_fseeki64(fin, 0, SEEK_END);
			fSize = _ftelli64(fin);
			rewind(fin);
			unsigned long long bSize = (CHARS_READ > fSize) ? fSize : CHARS_READ;
			fread(buffer, sizeof(unsigned char), bSize, fin);
			fclose(fin);

			// seek for 'Title:'
			unsigned long long titleStartPos = 2, titleEndPos = 0;
			while(titleEndPos == 0) {
				while((buffer[titleStartPos] != ':') && (titleStartPos < bSize)) titleStartPos++;
				if(titleStartPos == bSize) break; // wrong file structure
				titleStartPos += 2;
				if((buffer[titleStartPos-6] == 'i') && (buffer[titleStartPos-5] == 't') &&
					(buffer[titleStartPos-4] == 'l') && (buffer[titleStartPos-3] == 'e'))
				{
					titleEndPos = titleStartPos;
					bool noNewLine = true;
					while((titleEndPos < bSize) && ((noNewLine) || (buffer[titleEndPos] != ':'))) {
						if(buffer[titleEndPos] < ' ') {
							if(buffer[titleEndPos] == '\n')
								noNewLine = false;
							buffer[titleEndPos] = ' ';
						}
						titleEndPos++;
					}
					if(titleEndPos == bSize) break; // wrong file structure
					while(buffer[titleEndPos] >= ':') titleEndPos--; // go back before the label 'Something:'
					while(buffer[titleEndPos-1] == ' ') titleEndPos--; // remove all whitespaces
				}
			}

			if((titleStartPos != bSize) && (titleEndPos != bSize)) {
				filesIndexed++;
				TCHAR *title = new TCHAR[titleEndPos-titleStartPos+1];
				size_t titleLen = mbstowcs(title, (const char *)(buffer + titleStartPos), titleEndPos-titleStartPos);
				title[titleLen] = '\0';

				doc.add( *_CLNEW Field(_T("title"),
									title,
									Field::STORE_YES |
									Field::INDEX_TOKENIZED) );
				writer->addDocument(&doc);
				doc.clear();

				//printf("%s\n", inDir->path().string().c_str());
				delete title;
			}
		}

		writer->close();
		delete writer;

		// release resources
		delete[] buffer;
		dir->close();
		delete dir;
	
	} catch (CLuceneError& e) {
		_tprintf(_T(" caught a exception: %s\n"), e.twhat());
	} catch (...) {
		_tprintf(_T(" caught an unknown exception\n"));
	}

	return filesIndexed;
}

void useIndex(const char *indexDir, const TCHAR *queryStr) {
	SimpleAnalyzer analyzer;
	try {
		Directory *dir = FSDirectory::getDirectory(indexDir);
		IndexReader* reader = IndexReader::open(dir);
		IndexSearcher searcher(reader);
		QueryParser parser(_T("title"), &analyzer);
		parser.setPhraseSlop(4);
		Hits* hits = NULL;
        Query* query = parser.parse(queryStr);
        const wchar_t* qryInfo = query->toString(_T("title"));
        _tprintf(_T("Query: %s\n"), qryInfo);
        delete[] qryInfo;
        hits = searcher.search(query);
        _tprintf(_T("%d total results\n"), hits->length());
        for (size_t i=0; i < hits->length() && i<10; i++) {
			Document* d = &hits->doc(i);
			_tprintf(_T("#%d. %s (score: %f)\n"), i, d->get(_T("title")), hits->score(i));
        }
        delete hits;
        delete query;
		searcher.close(); reader->close(); delete reader;
		dir->close(); delete dir;
	} catch (CLuceneError& e) {
		_tprintf(_T(" caught a exception: %s\n"), e.twhat());
	} catch (...) {
		_tprintf(_T(" caught an unknown exception\n"));
	}
}



int main(int argc, char* argv[]) {

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	QueryPerformanceFrequency(&frequency);

	// build index
	if(BUILD_INDEX) {
		QueryPerformanceCounter(&t1);
		unsigned long long filesIndexed = buildIndex(INDEX_DIR, argv[1]);
		QueryPerformanceCounter(&t2);
		printf(" %llu files indexed in %.2lf ms.\n", filesIndexed, (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);
	}

	// use index
	QueryPerformanceCounter(&t1);
	useIndex(INDEX_DIR, _T(QUERY));
	QueryPerformanceCounter(&t2);
	printf(" Database queried in %.2lf ms.\n", (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart);

	return 0;
}