#include<locale>
#include<iostream>
#include<fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <Windows.h>
using namespace std;

#include<filesystem>

//this library is part C++17 but not completed in VS
//experimental means the library hasn't been finalized
using namespace std::experimental::filesystem;



class Fileusage {
public:

	string filename;
	int filespace = 0;
};

//predicate function to sort data by file_size
bool bySize(Fileusage const& lhs, Fileusage const& rhs) {

	return lhs.filespace < rhs.filespace;
}

//predicate function to sort data by filename
bool byName(Fileusage const& lhs, Fileusage const& rhs) {
	return lhs.filename < rhs.filename;
}

//int operator overload to sum the data from the class template<T> vector
unsigned int& operator += (unsigned int& sum, Fileusage const& f) {

	return sum += f.filespace;
}

//ostream operator overload to print formatted data
ostream& operator << (ostream& os, Fileusage const& f) {
	os << f.filespace << "       "  << f.filename << endl;

	return os;
}


//void regular_scan(path& f, Fileusage& fileinfo, vector<Fileusage>& v, unsigned int clusterSize);
void help_switch();
void recursive_scan(path& f, Fileusage& fileinfo, vector<Fileusage>& v, unsigned int clusterSize);

int main(int argc, char* argv[]) {

	cout << "\ndu.exe (c) Alistair Alva 2019 v 1.0.0" << endl;

	if (argc <= 1) {
		cerr << "Error, too few arguments" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
		exit(EXIT_FAILURE);
	}

	//vector of command-line-args
	vector<string> argsV(&argv[1], &argv[argc]);

	//if no arguments entered
	if (argsV.empty()) {
		cerr << "\nError: No arguments" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
		exit(EXIT_FAILURE);
	}

	//repeated arguments
	for (size_t i = 0; i < argsV.size(); ++i) {
		if (argv[i] == argsV[i]) {
			cerr << "\nError: Repeated arguments" << endl;
			cout << "txt2html, (c) 2019, Alistair Alva" << endl;
			cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
			exit(EXIT_FAILURE);
		}
	}

	//checking for repeated arguments
	for (size_t i = 0; i < argsV.size(); ++i) {
		for (size_t j = i + 1; j < argsV.size(); ++j) {
			if (argsV[i] == argsV[j]) {
				cerr << "\nError: Repeated arguments" << endl;
				cout << "txt2html, (c) 2019, Alistair Alva" << endl;
				cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
				exit(EXIT_FAILURE);
			}
		}
	}

	//switches
	bool s = false, b = false, h = false, z = false, n = false, r = false, k = false, help = false, blockSize = false, folder = false;
	
	//vector<char>switchVec;
	string foldernameStr = ".";
	string clusterSize; 


	//this is supposed to find repeated switches and check for unknown switches as well.
	//will set the swtich bool values to true if found
	for (size_t i = 0; i < argsV.size(); ++i) {
		if (argsV[i].at(0) == '-' && argsV[i].at(1) != '-') {
			string temp = argsV[i];
			for (size_t j = 1; j < temp.length(); ++j) {
				if (temp[j] != 's' && temp[j] != 'b' && temp[j] != 'h' && temp[j] != 'z' && temp[j] != 'k' && temp[j] != 'n' && temp[j] != 'r') {
					cerr << "\nError: Unknown switch <" << temp[j] << "> entered." << endl;
					cout << "txt2html, (c) 2019, Alistair Alva" << endl;
					cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]*";
					exit(EXIT_FAILURE);
				}
				for (size_t x = j + 1; x < temp.length(); ++x) {
					if (temp[j] == temp[x]) {
						cerr << "\nError: Repeated switches" << endl;
						cout << "txt2html, (c) 2019, Alistair Alva" << endl;
						cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]*";
						exit(EXIT_FAILURE);
					}
				}

				if (temp[j] == 's')
					s = true;
				if (temp[j] == 'b')
					b = true;
				if (temp[j] == 'h')
					h = true;
				if (temp[j] == 'z')
					z = true;
				if (temp[j] == 'k')
					k = true;
				if (temp[j] == 'r')
					r = true;

				//switchVec.push_back(temp[j]);
			}
		}
		if (argsV[i] != "--help") {
			//size_t index = 0;
			if (argsV[i].at(0) == '-' && argsV[i].at(1) == '-') {
				string temp = argsV[i];
				clusterSize = temp.substr(13, temp.length() - 1);
				blockSize = true;
			}
		}
		if (argsV[i] == "--help")
			help = true;

		if (i == 3 || i == 4) {
			path folderNameCheck = absolute(argsV[i]);
			if (!(exists(folderNameCheck))) {
				if (!(is_directory(folderNameCheck))) {
						cerr << "\nError: Filename <" << argsV[i] << "> does not exist. Please try again" << endl;
						cout << "txt2html, (c) 2019, Alistair Alva" << endl;
						cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]*";
						exit(EXIT_FAILURE);	
				}
			}

		}
		
		path foldername = absolute(argsV[i]);
		if (exists(foldername))
			foldernameStr = argsV[i];
	}

	//switch validation 

	if (b == true && h == true) {
		cerr << "\nError: Cannot use -b and -h switches together" << "Please try again" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]*";
		exit(EXIT_FAILURE);
	}

	if (k == true && blockSize == true) {
		cerr << "\nError: Cannot use -k and --block-size switches together" << "Please try again" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]*";
		exit(EXIT_FAILURE);
	}

	if (z == true && n == true) {
		cerr << "\nError: Cannot use -z and -n switches together" << "Please try again" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]*";
		exit(EXIT_FAILURE);
	}


	//using find function to locate switches
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

	//creating a vector of class template<T> type to be instantiated/filled
	vector<Fileusage> files;
	//class object
	Fileusage f1;
	//default cluster size
	unsigned int size = 4096;

	if (help == true)
		help_switch();

	//setting 
	if (k == true)
		size = 1024;
	
	if (blockSize == true) {
		size = stoi(clusterSize, nullptr, 0);
		if (size  <= 0) {
			cerr << "Error: Block size has to be greater than 0" << "Please try again" << endl;
			cout << "txt2html, (c) 2019, Alistair Alva" << endl;
			cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]";
			exit(EXIT_FAILURE);
		}
	}

	
	path currfolder = absolute(foldernameStr);
	
	//scan the directory
	recursive_scan(currfolder, f1, files, size);

	if (b == true) {
		for (size_t i = 0; i < files.size(); ++i)
			files[i].filespace = files[i].filespace * size;
	}
	
	
	if (h == true) {
		int tempVal = 0;
		for (size_t i = 0; i < files.size(); ++i)
		{
			//files[i].filespace = temp; 
			if ((files[i].filespace * size) / 1000000 > 0) {

			}
		}
	}

	//locale format
	std::cout.imbue(locale(""));

	if (s == true) {
		unsigned int sum = 0;
		//display summary
		for (size_t i = 0; i < files.size(); ++i) {
			sum += files[i].filespace;
		}
		cout << sum << "    " << currfolder.string() << endl;
		exit(EXIT_SUCCESS);
		
	}

	//sort by filespace
	if (z == true) {
		sort(files.begin(), files.end(), bySize);
	}

	//sort by filename
	if (n == true) {
		sort(files.begin(), files.end(), byName);
	}

	//reverse the rows
	if (r == true) {
		reverse(files.begin(), files.end());
   }

	//print all rows
	cout << "Scanning : " << currfolder.string() << " ..." << endl;

	for (size_t i = 0; i < files.size(); ++i)
		cout  << files[i];
}


/**
function: recursive_scan() 
Purpose: recursively scan the current directory and all subfolders
*/
void recursive_scan(path& f, Fileusage& fileinfo, vector<Fileusage>& v, unsigned int clusterSize)
{
	recursive_directory_iterator dir(f);
	recursive_directory_iterator end;

	int filesize = 0;
	
	if (!(is_directory(f))) {
		cerr << "\nError: Filename <" << f << "> does not exist. Please try again" << endl;
		cout << "txt2html, (c) 2019, Alistair Alva" << endl;
		cout << "Usage: du [-s(b|h)(z|n)rk]|[--help] [--block-size=dddd] [root folder]*";
		exit(EXIT_FAILURE);
	}

	//begin the search
	while (dir != end)
	{
		if (is_regular_file(dir->status())) {
			//fileName = dir->path().filename().string();
			fileinfo.filename = dir->path().filename().string();
			//cout << ", filename = " << dir->path().filename();
			filesize = (file_size(dir->path()));
			filesize = filesize / clusterSize;
			fileinfo.filespace = filesize;
		}
		//Fileusage fileInfo(fileName, filesize);
		v.push_back(fileinfo);
		++dir;
	}
}

/**
Function: help();
Purpose: will display intructions if the help switch is pressed
*/
void help_switch() {
	 cout << "du(c) 2019, Alistair Alva"
		<< "\n==========================================================="
		<< "\nVersion 1.0.0"

		 << "\nA disk usage utility inspired by the UNIX du command."

		 << "\nUsage: du[-skhb][--help][--block - size = dddd][folder] *"

		 "\nExamples :"
		 " \n du"
	"\n  > display the sum of the cluster sizes of each directory starting the cwd"

		 "\n du folder"
	"\n > display the sum of the cluster sizes of each directory"
		 "starting with 'folder'"

		 "\n du - h"
	"\n  > display the results in a human readable form"

		 "\n du - s"
	"\n  > display only the final summary"

		 "\n du - b"
	"\n  > display in bytes"

		 "\n du - k"
	"\n  > cluster size is 1024"

		 "\n du - z"
	"\n  > display the list sorted by size"

		 "\n du - n"
	"\n  > display the list sorted by name"

		 "\n du - r"
	"\n  > display the list in reverse order"

		 "\n du --block - size = dddd"
	"\n  > set the cluster size to the specified integer > 0"

		 "\n du --help"
	"\n  > displays the help" << endl;

	 exit(EXIT_SUCCESS);
}
