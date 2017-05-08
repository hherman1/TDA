// Vec2DistanceMat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <set>
#include <sstream>
#include <iterator>

//#define DEBUG

using namespace std;
const int MAX_LINE_SIZE = 2048*8;

struct entry {
	string word;
	vector<float> elems;
};

void cleanFile(string filename, string outfile);
void distanceMatrix(string vecFile, string outFile);
void helpDialog();
set<string> constructFilter(string filename);
set<string> wordList(string sourceFile);
void filterVecFile(string filename, string filtername, string outfile);
void complementFilter(string filterFile, string sourceFile, string outfile);
void writeSet(set<string> wordSet, string outDest);



// grabbed from SO
// http://stackoverflow.com/questions/5888022/split-string-by-single-spaces
unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
	unsigned int pos = txt.find(ch);
	unsigned int initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

float square_difference(float x, float y) {
	return (x - y)*(x - y);
}

int main(int argc, char*argv[])
{
#ifdef DEBUG
	//cleanFile("C:\\Users\\thepr\\Code\\Topology\\data\\Phenomenology of Spirit - G.W.F.Hegel.txt", "C:\\Users\\thepr\\Code\\Topology\\data\\PoS.clean");
	
	//cleanFile("C:\\Users\\thepr\\Code\\Topology\\data\\Phenomenology of Spirit - G. W. F. Hegel.txt", "C:/Users/thepr/Code/Topology/data/PoS.clean");
	//constructFilter("C:\\Users\\thepr\\Code\\Topology\\data\\PoS.filter");
	
	filterVecFile("C:\\Users\\thepr\\Code\\Topology\\data\\warden.clean.vec", "C:/Users/thepr/Code/Topology/data/PoS.filter.clean", "C:\\Users\\thepr\\Code\\Topology\\data\\PoS.vec.clean.filtered");
	//distanceMatrix("C:\\Users\\thepr\\Code\\Topology\\data\\PoS.vec.clean.filtered", "C:/Users/thepr/Code/Topology/data/PoS.mat.clean.filtered");
	//complementFilter("C:\\Users\\thepr\\Code\\Topology\\data\\PoS.filter.clean", "C:/Users/thepr/Code/Topology/data/PoS.clean","C:/Users/thepr/Code/Topology/data/PoS.clean.filter.complement");
#endif // DEBUG
	if (argc <= 1) {
		helpDialog();
	}
	else if (argc > 1 && string(argv[1]) == "-c") {
		cleanFile(argv[2], argv[3]);
	} else 	if (argc > 1 && string(argv[1]) == "-v2m") {
		distanceMatrix(argv[2], argv[3]);
	}
	else if (argc > 1 && string(argv[1]) == "-h") {
		helpDialog();
	}
	else if (argc > 1 && string(argv[1]) == "-f") {
		filterVecFile(argv[2], argv[3], argv[4]);
	}
	else if (argc > 1 && string(argv[1]) == "-fi") {
		filterVecFile(argv[2], argv[3], argv[4]);
	}
	else if (argc > 1 && string(argv[1]) == "-complement") {
		complementFilter(argv[2], argv[3], argv[4]);
	}
	else if (argc > 1 && string(argv[1]) == "-unique") {
		set<string> uniques = wordList(argv[2]);
		writeSet(uniques, argv[3]);
	}
#ifdef DEBUG
	int trash;
	cin >> trash;
#endif // DEBUG
    return 0;
}

void helpDialog() {
	cout << endl << "Help: " << endl
		<< "\t -h: Help dialog." << endl
		<< "\t -v2m <infile> <outfile>: Convert infile (word2vec output) to distance matrix. " << endl
		<< "\t -f <vector file> <filter file> <output file>: Keep only the vectors listed in the filter file. " << endl
		<< "\t -fi <vector file> <indicesfilter file> <output file>: Keep only the vectors listed in the filter file. (listed by index) " << endl
		<< "\t -c <inflie> <outfile>: Remove all punctuation and lowercase letters of input text file. " << endl
		<< "\t -complement <filter file> <clean source file> <outfile>: Constructs a complement filter file to the filter file in the source document. " << endl
		<< "\t -unique <source file> <outfile>: Constructs a filter file containing all of the unique words in the source file. " << endl << endl;
}


void complementFilter(string filterFile, string sourceFile, string outfile) {
	set<string> filter = constructFilter(filterFile);
	cout << "Read filter." << endl;
	set<string> sourceWords = wordList(sourceFile);
	cout << "Found unique words." << endl;
	set<string> complement;
	set_difference(sourceWords.begin(), sourceWords.end(), filter.begin(), filter.end(), std::inserter(complement, complement.end()));
	writeSet(complement, outfile);
	cout << "Complement written." << endl;


}

void writeSet(set<string> wordSet, string outDest) {
	ofstream ofs;
	ofs.open(outDest);
	for (auto i = wordSet.begin(); i != wordSet.end(); i++) {
		ofs << *i << endl;
	}
	ofs.close();
}

set<string> wordList(string sourceFile) {
	ifstream ifs;
	ifs.open(sourceFile);
	if (!ifs.is_open()) {
		string error;
		cerr << "Error: " << strerror(errno) << " " << sourceFile << endl;
		/*error = "Failed to open file: " + filename + "; Error: " + strerror(errno);*/
		throw error;
	}
	set<string> out;

	string word;
	string line;
	while(!ifs.eof()) {
#ifdef DEBUG

		if (ifs.bad()) {
			cerr << "Error: " << strerror(errno) << " " << sourceFile << endl;
			/*error = "Failed to open file: " + filename + "; Error: " + strerror(errno);*/
			throw;
		}
		if (ifs.fail()) {
			cerr << "Error: " << strerror(errno) << " " << sourceFile << endl;
			/*error = "Failed to open file: " + filename + "; Error: " + strerror(errno);*/
			throw;
		}
		if (ifs.eof()) {
			cerr << "Error: " << strerror(errno) << " " << sourceFile << endl;
			/*error = "Failed to open file: " + filename + "; Error: " + strerror(errno);*/
			throw;
		}
#endif // DEBUG

		getline(ifs,line);
		stringstream linestream(line);
		while (getline(linestream, word, ' ')) {
			out.insert(word);
		}

	}


	ifs.close();
	return out;
}

void distanceMatrix(string vecFile, string outFile) {
	int ret;

	ifstream ifs;
	ifs.open(vecFile);
	if (!ifs.is_open()) {
		string error;
		cerr << "Error: " << strerror(errno) << " " << vecFile << endl;
		/*error = "Failed to open file: " + filename + "; Error: " + strerror(errno);*/
		throw error;
	}
	//char line[MAX_LINE_SIZE];

	char line[MAX_LINE_SIZE];
	//char line[MAX_LINE_SIZE];

	//getline(ifs, line);
	ifs.getline(line, MAX_LINE_SIZE);

	vector<string> options;
	split(line, options, ' ');
	const int numWords = stoi(options[0]);
	const int vecLength = stoi(options[1]);
	cout << "Number of words: " << numWords << endl;
	cout << "Vector length: " << vecLength << endl;


	vector<entry> entries(numWords);

	int wordIndex = 0;
	char * pch;
	char * delimeter = " ";
	while (wordIndex < numWords) {
		ifs.getline(line, MAX_LINE_SIZE);
		pch = strtok(line, delimeter);
		string word(pch);
		//cout << pch << endl; 
		// initial word

		vector<float> vec(vecLength);
		for (int i = 0; i < vecLength; i++) {
			pch = strtok(NULL, delimeter);
			vec[i] = atof(pch);
		}
		//pch = strtok(NULL, delimeter);

		entries[wordIndex] = { word, vec };
		wordIndex++;
	}
	ifs.close();

	// write distance matrix
	ofstream results;
	results.open(outFile);


	vector<float> distMat(numWords*numWords);
	for (int i = 0; i < numWords; i++) {
		for (int j = 0; j < numWords; j++) {
			float dist = sqrt(inner_product(entries[i].elems.begin(), entries[i].elems.end(), entries[j].elems.begin(), 0.f,
				std::plus<float>(), square_difference));
			results << dist << ' ';
		}
		results << endl;
	}
	results.close();
	cout << "Finished" << endl;
#ifdef DEBUG
	cin >> ret;
#endif // DEBUG
}

static string punctuation = ",.<>;:\"'/?!@#$%^&*()=+_[]|{}~`";
void cleanString(string & in) {
	for (unsigned int i = 0; i < punctuation.size(); ++i)
	{
		// you need include <algorithm> to use general algorithms like std::remove()
		in.erase(std::remove(in.begin(), in.end(), punctuation[i]), in.end());
	}
	std::transform(in.begin(), in.end(), in.begin(), tolower);
}

void cleanFile(string filename,string outfile) {
	ifstream fi;
	ofstream fo;
	fi.open(filename);
	fo.open(outfile);
	if (!fi.is_open()) {
		string error;
		cerr << "Error: " << strerror(errno) << " " << filename << endl;
		/*error = "Failed to open file: " + filename + "; Error: " + strerror(errno);*/
		throw error;
	}
	string line;
	while (!fi.eof()) {
		getline(fi, line);
		cleanString(line);
#ifdef DEBUG
		//cout << line << endl;
#endif // DEBUG

		fo << line << endl;
	}
	fi.close();
	fo.close();
}

void filterVecFile(string filename, string filtername, string outfile) {
	set<string> filter = constructFilter(filtername);
	ifstream ifs;
	ifs.open(filename);
	ofstream ofs;
	ofs.open(outfile);


	string firstline;
	getline(ifs, firstline);
	
	//ofs << filter.size() << firstline.substr(firstline.find(' ')) << endl;

	stringstream ss;

	string line;

	int entries = 0;

#ifdef DEBUG
	set<string> found;
#endif // DEBUG


	while (!ifs.eof())
	{
		getline(ifs, line);
		string word = line.substr(0, line.find(' '));
		if (word.size() > 0 && filter.find(word) != filter.end()) {
			ss << line << endl;
			entries++;
#ifdef DEBUG
			found.insert(word);
#endif // DEBUG

		}
	}

	ofs << entries << firstline.substr(firstline.find(' ')) << endl;
	ofs << ss.str();
	ofs.close();
	ifs.close();

#ifdef DEBUG
	set<string> result;
	set_difference(filter.begin(), filter.end(), found.begin(), found.end(), std::inserter(result,result.end()));
	for (auto i = result.begin(); i != result.end(); i++) {
		cout << (*i) << endl;
	}
	int t;
	cin >> t;
#endif // DEBUG

}

set<string> constructFilter(string filename) {
	ifstream f;
	f.open(filename);
	string line;
	set<string> out;
	while (!f.eof()) {
		getline(f, line);
		//string word = line.substr(0,line.find(' '));
		cleanString(line);
		out.insert(line);
	}
	f.close();
	return out;
}

vector<entry> filterEntries(set<string> filter, vector<entry> entries) {
	vector<entry> result(filter.size());
	auto j = result.begin();
	for (auto i = entries.begin(); i < entries.end(); i++) {
		if (filter.find((*i).word) != filter.end()) {
			*j = *i;
			j++;
		}
	}
	return result;
}