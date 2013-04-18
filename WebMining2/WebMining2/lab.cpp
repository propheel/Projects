#include <algorithm>
#include <string>
#include <hash_map>
#include <sstream>
#include <vector>

#define REAL float
#define FILES 15

std::string stopWords[] = { "a", "aby", "ach", "acz", "aczkolwiek", "aj", "albo", "ale", "ale�", "ani", "a�", "bardziej", "bardzo", "bo", "bowiem", "by", "byli", "bynajmniej", "by�", "by�", "by�a", "by�o", "by�y", "b�dzie", "b�d�", "cali", "ca�a", "ca�y", "ci", "ci�", "ciebie", "co", "cokolwiek", "co�", "czasami", "czasem", "czemu", "czy", "czyli", "daleko", "dla", "dlaczego", "dlatego", "do", "dobrze", "dok�d", "do��", "du�o", "dwa", "dwaj", "dwie", "dwoje", "dzi�", "dzisiaj", "gdy", "gdyby", "gdy�", "gdzie", "gdziekolwiek", "gdzie�", "go", "i", "ich", "ile", "im", "inna", "inne", "inny", "innych", "i�", "ja", "j�", "jak", "jaka�", "jakby", "jaki", "jakich�", "jakie", "jaki�", "jaki�", "jakkolwiek", "jako", "jako�", "je", "jeden", "jedna", "jedno", "jednak", "jednak�e", "jego", "jej", "jemu", "jest", "jestem", "jeszcze", "je�li", "je�eli", "ju�", "j�", "ka�dy", "kiedy", "kilka", "kim�", "kto", "ktokolwiek", "kto�", "kt�ra", "kt�re", "kt�rego", "kt�rej", "kt�ry", "kt�rych", "kt�rym", "kt�rzy", "ku", "lat", "lecz", "lub", "ma", "maj�", "ma�o", "mam", "mi", "mimo", "mi�dzy", "mn�", "mnie", "mog�", "moi", "moim", "moja", "moje", "mo�e", "mo�liwe", "mo�na", "m�j", "mu", "musi", "my", "na", "nad", "nam", "nami", "nas", "nasi", "nasz", "nasza", "nasze", "naszego", "naszych", "natomiast", "natychmiast", "nawet", "ni�", "nic", "nich", "nie", "niego", "niej", "niemu", "nigdy", "nim", "nimi", "ni�", "no", "o", "obok", "od", "oko�o", "on", "ona", "one", "oni", "ono", "oraz", "oto", "owszem", "pan", "pana", "pani", "po", "pod", "podczas", "pomimo", "ponad", "poniewa�", "powinien", "powinna", "powinni", "powinno", "poza", "prawie", "przecie�", "przed", "przede", "przedtem", "przez", "przy", "roku", "r�wnie�", "sam", "sama", "s�", "si�", "sk�d", "sobie", "sob�", "spos�b", "swoje", "ta", "tak", "taka", "taki", "takie", "tak�e", "tam", "te", "tego", "tej", "ten", "teraz", "te�", "to", "tob�", "tobie", "tote�", "trzeba", "tu", "tutaj", "twoi", "twoim", "twoja", "twoje", "twym", "tw�j", "ty", "tych", "tylko", "tym", "u", "w", "wam", "wami", "was", "wasz", "wasza", "wasze", "we", "wed�ug", "wiele", "wielu", "wi�c", "wi�cej", "wszyscy", "wszystkich", "wszystkie", "wszystkim", "wszystko", "wtedy", "wy", "w�a�nie", "z", "za", "zapewne", "zawsze", "ze", "z�", "znowu", "zn�w", "zosta�", "�aden", "�adna", "�adne", "�adnych", "�e", "�eby" };
int stopWordsLength = 276;
std::string workFolder = "C:\\Users\\Filip\\Documents\\Visual Studio 2010\\Projects\\WebMining2\\WebMining2\\dane\\";
std::string fileNames[] = { "inf1.txt", "inf2.txt", "inf3.txt", "inf4.txt", "inf5.txt",
							"muz1.txt", "muz2.txt", "muz3.txt", "muz4.txt", "muz5.txt",
							"zoo1.txt", "zoo2.txt", "zoo3.txt", "zoo4.txt", "zoo5.txt" };

REAL measure;
REAL vecLen[FILES];
std::vector<REAL> weights[FILES];
std::vector<int> counts;
stdext::hash_map<std::string, int> dict;
stdext::hash_map<std::string, int>::iterator dictIt;
std::vector<std::pair<REAL, std::string> > results;
char buffer[1000];
int ctrl, wordsInFile, dictSize;

int main(int argc, char* argv[]) {

	dict.clear();
	FILE *fin;
	// build a dictionary
	printf("Building dictionary...\n");
	for(int fileId=0; fileId<FILES; fileId++) {
		if(fopen_s(&fin, (workFolder + fileNames[fileId]).c_str(), "r") != 0)
			return 1;
		ctrl = fscanf_s(fin, "%s", buffer, 1000);
		while(!feof(fin)) {
			if(ctrl < 1) return 2;
			if(dict.find(std::string(buffer)) == dict.end())
				dict.insert(std::make_pair(std::string(buffer), dict.size()));
			ctrl = fscanf_s(fin, "%s", buffer, 1000);
		}
		fclose(fin);
	}
	printf("%d keys - erasing stop words...\n", dict.size());
	dictSize = dict.size();
	for(int i=0; i<stopWordsLength; i++)
		if(dict.find(stopWords[i]) != dict.end())
			dict.erase(dict.find(stopWords[i]));
	printf("%d keys in the dictionary.\nMaking vectors...\n", dict.size());
	counts.reserve(dictSize);
	results.clear();
	for(int fileId=0; fileId<FILES; fileId++) {
		if(fopen_s(&fin, (workFolder + fileNames[fileId]).c_str(), "r") != 0)
			return 1;
		wordsInFile = 0;
		weights[fileId].reserve(dictSize);
		weights[fileId].clear();
		weights[fileId].insert(weights[fileId].begin(), dictSize, 0);
		counts.clear();
		counts.insert(counts.begin(), dictSize, 0);
		ctrl = fscanf_s(fin, "%s", buffer, 1000);
		while(!feof(fin)) {
			if(ctrl < 1) return 2;
			if((dictIt = dict.find(std::string(buffer))) != dict.end()) {
				counts[dictIt->second]++;
				wordsInFile++;
			}
			ctrl = fscanf_s(fin, "%s", buffer, 1000);
		}
		fclose(fin);
		vecLen[fileId] = 0;
		for(int i=0; i<dictSize; i++) {
			weights[fileId][i] = (REAL)counts[i] / (REAL)wordsInFile;
			vecLen[fileId] += (weights[fileId][i]) * (weights[fileId][i]);
		}
		vecLen[fileId] = sqrt(vecLen[fileId]);

		for(int compFI = 0; compFI < fileId; compFI++) {
			measure = 0;
			for(int i=0; i<dictSize; i++)
				measure += weights[fileId][i] * weights[compFI][i];
			measure /= vecLen[fileId] * vecLen[compFI];
			results.push_back(make_pair(measure, fileNames[fileId] + " - " + fileNames[compFI]));
		}
	}
	printf("Sorting measures...\n");
	std::sort(results.begin(), results.end());
	printf("Showing 10 best matches:\n");
	for(int i=results.size()-1; ((i>results.size()-11) && (i>-1)); i--)
		printf("%s : %.4f\n", results[i].second.c_str(), results[i].first);
	printf("Showing 10 worst matches:\n");
	for(int i=0; ((i<results.size()) && (i<10)); i++)
		printf("%s : %.4f\n", results[i].second.c_str(), results[i].first);
	return 0;
}