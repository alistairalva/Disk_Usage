#include<locale>
#include<iostream>
#include<fstream>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <algorithm>
#include <map>
#include <Windows.h>
using namespace std;

#include<filesystem>
//this library is part C++17 but not completed in VS
//experimental means the library hasn't been finalized
using namespace std::experimental::filesystem;

void regular_scan(path const& f, Fileusage fileinfo, vector<Fileusage> v);

class Fileusage {
public:

	string filename;
	unsigned long int filespace;
};

int main(int argc, char* argv[]) {

	cout << "du.exe (c) Alistair Alva 2019 v 1.0.0" << endl;

	if (argc <= 1) {
		cerr << "Error, too few arguments" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
		exit(EXIT_FAILURE);
	}

	vector<string> argsV(&argv[1], &argv[argc]);

	if (argsV.empty()) {
		cerr << "Error: No arguments" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < argsV.size(); ++i) {
		if (argv[i] == argsV[i]) {
			cerr << "Error: Repeated arguments" << endl;
			cout << "txt2html, (c) 2019, Alistair Alva" << endl;
			cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
			exit(EXIT_FAILURE);
		}
	}

	for (size_t i = 0; i < argsV.size(); ++i) {
		for (size_t j = i + 1; j < argsV.size(); ++j) {
			if (argsV[i] == argsV[j]) {
				cerr << "Error: Repeated arguments" << endl;
				cout << "txt2html, (c) 2019, Alistair Alva" << endl;
				cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
				exit(EXIT_FAILURE);
			}
		}
	}

	//switches
	bool s = false, b = false, h = false, z = false, n = false, r = false, k = false, help = false, blockSize = false, folder = false, recursion = false;
	
	//vector<char>switchVec;
	string foldernameStr;
	string clusterSize; 
	//this is supposed to find repeated switches and check for unknown switches
	for (size_t i = 0; i < argsV.size(); ++i) {
		if (argsV[i].at(0) == '-' && argsV[i].at(1) != '-') {
			string temp = argsV[i];
			for (size_t j = 1; j < temp.length(); ++j) {
				if (temp[j] != 's' && temp[j] != 'b' && temp[j] != 'h' && temp[j] != 'z' && temp[j] != 'k') {
					cerr << "Error: Unknown switch <" << temp[j] << "> entered." << endl;
					cout << "txt2html, (c) 2019, Alistair Alva" << endl;
					cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
					exit(EXIT_FAILURE);
				}
				for (size_t x = j + 1; x < temp.length(); ++x) {
					if (temp[j] == temp[x]) {
						cerr << "Error: Repeated switches" << endl;
						cout << "txt2html, (c) 2019, Alistair Alva" << endl;
						cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
						exit(EXIT_FAILURE);
					}
				}

				if (temp[j] == 's')
					s = true;
				if (temp[j] == 'b')
					b == true;
				if (temp[j] == 'h')
					h = true;
				if (temp[j] == 'z')
					z = true;
				if (temp[j] == 'k')
					k = true;
				if (temp[j] == 'r')
					r == true;

				//switchVec.push_back(temp[j]);
			}
		}
		if (argsV[i].at(0) == '-' && argsV[i].at(1) == '-') {
			string temp = argsV[i];
			clusterSize = temp.substr(2, temp.length() - 1);
			blockSize = true;
		}

		if (argsV[i] == "--help")
			help = true;

		if (i == 4 || i == 3) {
			path folderNameCheck = absolute(argsV[i]);
			if (!(exists(folderNameCheck))) {
				if (!(is_directory(folderNameCheck))) {
					if (!(is_regular_file(folderNameCheck))) {
						cerr << "Error: Filename <" << argsV[i] << "> does not exist. Please try again" << endl;
						cout << "txt2html, (c) 2019, Alistair Alva" << endl;
						cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
						exit(EXIT_FAILURE);
					}
				}
			}
		}
		
		path foldername = absolute(argsV[i]);
			if (exists(foldername)) {
				foldernameStr = argsV[i];
				if (is_directory(foldername))
					recursion = true;
			}
	}

	if (b == true && h == true) {
		cerr << "Error: Cannot use -b and -h switches together" << "Please try again" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
		exit(EXIT_FAILURE);
	}

	if (k == true && blockSize == true) {
		cerr << "Error: Cannot use -k and --block-size switches together" << "Please try again" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
		exit(EXIT_FAILURE);
	}

	if (z == true && n == true) {
		cerr << "Error: Cannot use -z and -n switches together" << "Please try again" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
		exit(EXIT_FAILURE);
	}

	vector<Fileusage> f1;

	//using find function
	/*vector<char>::iterator pS = find(switchVec.begin(), switchVec.end(), 'z');
	if (pS != argsV.end())
		s == true;

	vector<char>::iterator pB = find(switchVec.begin(), switchVec.end(), 'b');
	if (pB != argsV.end())
		b == true;

	vector<char>::iterator pH = find(switchVec.begin(), switchVec.end(), 'h');
	if (pH != argsV.end())
		h == true;

	vector<char>::iterator pZ = find(switchVec.begin(), switchVec.end(), 'z');
	if (pZ != argsV.end())
		z == true;

	vector<char>::iterator pN = find(switchVec.begin(), switchVec.end(), 'n');
	if (pN != argsV.end())
		n == true;

	vector<char>::iterator pR = find(switchVec.begin(), switchVec.end(), 'r');
	if (pR != argsV.end())
		r == true;

	vector<char>::iterator pK = find(switchVec.begin(), switchVec.end(), 'k');
	if (pK != argsV.end())
		k == true;*/

	//help switch in the main vector
	/*vector<string>::iterator pHelp = find(argsV.begin(), argsV.end(), "--help"); 
	if (pHelp != argsV.end())
		help == true;*/
}

void regular_scan(path const& f, Fileusage fileinfo, vector<Fileusage> v) {
		//use iterators to walk through the folder, like iterators used for containers 
		//create a beginning directory iterator passing it the path object, which represents the root of the search
		directory_iterator dir(f);


		//create an end iterator, points to the end of a search which is basically the end of any folder
		directory_iterator end;

		unsigned long int filesize = 0;
		//begin the search
		//continuous reading until the end of the folder 
		while (dir != end) {
			//print what ierator is looking at, either folder or a file
			//cout << "path = " << dir->path();
			/*if (is_directory(dir->status())) {
				cout << " [dir] " << endl;
			}*/
			if (is_regular_file(dir->status())) {
				fileinfo.filename =  dir->path();
				cout << ", filename = " << dir->path().filename();
				fileSize = file_size(dir->path());

			}
			++dir;
	    }
}

//recursively scan the current folder and all subfolders
void rscan(path const& f)
{
	recursive_directory_iterator dir(f);
	recursive_directory_iterator end;

	//begin the search
	while (dir != end)
	{
		cout << "path = " << dir->path();

		if (is_directory(dir->status()))
			cout << " [dir] " << endl;
		else if (is_regular_file(dir->status()))
		{
			cout << ", filename = " << dir->path().filename();

			//research how to get the filesize
			cout << ", file size = " << endl;
		}

		++dir;
	}
}