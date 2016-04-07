#include <stdio.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <sstream>
using namespace std;
// ToDo Read template and insert in every folder / subdirectory
void findSubDirectoriesAndFiles(std::string dir, std::vector<std::string>& a)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile((dir + "/*").c_str(), &FindFileData);
	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("\nFindFirstFile failed (%d)\n", GetLastError());
		return;
	}
	else
	{
		while (FindNextFile(hFind, &FindFileData))
		{
			cout << "Found File: " << FindFileData.cFileName << endl;
			string test(FindFileData.cFileName);
			std::string nextDir = dir + "/" + test;
			
			if (!(test.find(".") != std::string::npos))	{
				cout << "Found new subdirectory" << endl;
				findSubDirectoriesAndFiles(nextDir, a);
			}
			else if (nextDir.find(".hpp") != std::string::npos)	{
				cout << "Found hpp file" << endl;
				a.push_back(nextDir);
			}
			else if (nextDir.find(".cpp") != std::string::npos)	{
				cout << "Found cpp file" << endl;
				a.push_back(nextDir);
			}
		}
		FindClose(hFind);
	}
	//return nextDir;
}

vector<string> getSubDirectories(string dir)	{
	std::vector<std::string> files;
	std::vector<std::string> correctFiles;
	std::vector<std::string> failedFiles;
	//std::string resultString;

	findSubDirectoriesAndFiles(dir, files);
	//resultString += "\n_______________________\nLoading images into textures, FILE LOCATION[" + location + "]...\n";
	cout << "header files are: " << endl;
	for (string& str : files)
	{
		cout << str << endl;
		//if (!newTexture->loadFromFile(str));
			//failedFiles.push_back(new std::string(printFileName(str)));
	}
	return files;
}
void changeDate(string& template_content)	{
	std::size_t foundDate = template_content.find("<dd-mm-yy>");
	time_t givenTime = time(0);
	struct tm * now = localtime( & givenTime );
	stringstream ss;
	ss << "<" << now->tm_mday << "-" << (now->tm_mon + 1) << "-" << (now->tm_year + 1900 - 2000) << ">";
	string currentDate;
	ss >> currentDate;
	template_content.replace(foundDate, 10, currentDate.c_str());
}

void changeAuthor(string& template_content, string& file)	{
	std::size_t foundAuthorField = template_content.find("<full name>");
	if(foundAuthorField == string::npos){
		return;
	} 
	std::size_t foundAuthorTag = file.find("@author");
	if(foundAuthorTag == string::npos){
		return;
	} 
	foundAuthorTag += 7;
	while(file.at(foundAuthorTag) < 'A' || file.at(foundAuthorTag) > 'z')	{
		cout << "file at: " << foundAuthorTag << endl;
		++foundAuthorTag;
	}
	string authorName("<");
	uint8_t authorNameLength = 0;
	// get name
	while(file.at(foundAuthorTag) >= 'A' && file.at(foundAuthorTag) <= 'z')	{
		cout << file.at(foundAuthorTag);
		authorName += file.at(foundAuthorTag);
		++authorNameLength;
		++foundAuthorTag;
	}
	cout << endl;
	authorName += ">";
	template_content.replace(foundAuthorField, authorNameLength, authorName);
		
}
int main(int argc, char* argv[])
{
	string t_file;
	if(argc == 1)
		t_file = "template.txt";
	else	{
		const char* templateFile = argv[1];
		t_file = templateFile;
	}
	cout << "template file is: " << t_file << endl;
	
	std::ifstream ifs(t_file);
	std::string template_content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
	ifs.close();
	cout << template_content;
	changeDate(template_content);
	
	
	for (string& str : getSubDirectories("../"))	{
		cout << "Writing into file..." << endl;
		ofstream oFile(str + "format");
		ifstream iFile(str);
		std::string hpp_content( (std::istreambuf_iterator<char>(iFile) ),
								(std::istreambuf_iterator<char>()    ) );
		changeAuthor(template_content, hpp_content);

		std::size_t found = hpp_content.find("++--++");
		if(found == string::npos)	{
			oFile << template_content << endl; // write template
			oFile << hpp_content;
			iFile.close();
			oFile.close();
			string f1 = str + "format";
			string f2 = str + "renamed";
			  if( remove( str.c_str() ) != 0 )
				perror( "Error deleting file" );
			  else
				puts( "File successfully deleted" );
			rename(f1.c_str(), str.c_str());
		}
		else {
			iFile.close();
			oFile.close();
			if( remove( (str + "format").c_str() ) != 0 )
				perror( "Error deleting file" );
			  else
				puts( "File successfully deleted" );
		}
		
	}

	// Read template file
	return 0;
}
