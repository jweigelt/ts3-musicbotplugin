/*
* This file is part of Music Bot Plugin. (https://github.com/jweigelt/ts3-musicbotplugin)
* Copyright (C) 2017 Jan Weigelt (jan@janelo.net)
*
* Music Bot Plugin is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* Music Bot Plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with Music Bot Plugin. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <stdio.h>
#include <fstream>
#include <string>
#include <SimpleIni.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <regex>

#include "Logger.h"
#include "ConfigException.h"
#include "Utility.h"

using namespace std;

#ifdef _WIN32
#include <direct.h>  
#define MKDIR(a) _mkdir(a)
#else
#define MKDIR(a) mkdir(a, 0733)
#endif

class ConfigReader
{
public:
	ConfigReader();
	~ConfigReader();

	void ReadFile(const char * path);
	void WriteFile(const char * path);
	bool FileExists(const char* path);
	bool DirectoryExists(const char* path);
	void CreateDirectoryStruct(const char* path);

	void SetBool(const char* section, const char* name, const char* comment, const bool &value);
	bool GetBool(const char* section, const char* name);

	void SetInt(const char* section, const char* name, const char* comment, const int32_t &value);
	int32_t GetInt(const char* section, const char* name);

	void SetString(const char* section, const char* name, const char* comment, const string &value);
	const string GetString(const char* section, const char* name);
	vector<string> GetAdditionalSections();

	bool SectionExists(const char* section);

private:
	CSimpleIniA ini;

};