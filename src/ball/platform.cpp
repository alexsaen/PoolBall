/*  
	Copyright (c) 2012, Alexey Saenko
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/ 

#include "platform.h"

#ifdef WIN32

#include <windows.h>
#include <fstream>

namespace platform {

static std::string settingsFilename;

bool fileExists(const char* filename, bool dir) {
    WIN32_FIND_DATAA fd;
    if(FindFirstFileA(filename, &fd)==INVALID_HANDLE_VALUE) 
		return false;
    if( ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0) ^ dir ) 
		return false;
    return true;
}

bool createDir(std::string dirName) {
	if(!fileExists(dirName.c_str(), true)) 
		return CreateDirectoryA(dirName.c_str(), 0)!=0;
	return true;
}

std::string	loadSettings() {
	std::string dataDir = std::string(getenv("APPDATA")) + "\\Roots";
	createDir(dataDir);
	settingsFilename = dataDir + "\\settings";
	std::ifstream is(settingsFilename.c_str());
	std::string result;
	if(!is)
		return result;
	while(!is.eof()) {
		std::string s;
		std::getline(is, s);
		result += s + "\n";
	}
	return result;
}

void saveSettings(const std::string &data) {
	if(settingsFilename.empty())
		return;
	std::ofstream os(settingsFilename.c_str());
	os << data;
}

};

#elif defined(LINUX)

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fstream>

namespace platform {

static std::string settingsFilename;

bool fileExists(const char* filename, bool dir) {
	struct stat st;
	return ((stat(filename, &st) == 0) && dir == (((st.st_mode) & S_IFMT) == S_IFDIR));
}

bool createDir(std::string dirName) {
	if(!fileExists(dirName.c_str(), true))
		return mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
	return true;
}

std::string	loadSettings() {
	char const* homedir = getenv("HOME");
	if(!homedir) {
		struct passwd *pw = getpwuid(getuid());
		homedir = pw->pw_dir;
	}
	std::string dataDir = std::string(homedir) + "/.roots";
	createDir(dataDir);
	settingsFilename = dataDir + "/settings";
	std::ifstream is(settingsFilename.c_str());
	std::string result;
	if(!is)
		return result;
	while(!is.eof()) {
		std::string s;
		std::getline(is, s);
		result += s + "\n";
	}
	return result;
}

void saveSettings(const std::string &data) {
	if(settingsFilename.empty())
		return;
	std::ofstream os(settingsFilename.c_str());
	os << data;
}

};


#elif defined(ANDROID)

// relication in native.cpp

#endif
