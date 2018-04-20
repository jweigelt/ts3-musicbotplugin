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
#include "ConfigReader.h"

ConfigReader::ConfigReader()
{ }

ConfigReader::~ConfigReader()
{ }

void ConfigReader::ReadFile(const char* path)
{
	if (!FileExists(path)) {
		CreateDirectoryStruct(path);
		Logger.Log(LogLevel_INFO, LOGSENDER_CONFIG, "Creating default configuration file: '%s'", path);
		WriteFile(path);
		return;
	}

	Logger.Log(LogLevel_DEBUG, LOGSENDER_CONFIG, "Loading '%s'", path);
	SI_Error err;
	if ((err = ini.LoadFile(path)) != SI_OK) {
		throw ConfigException("Failed to parse ini file", err);
	}
}

void ConfigReader::WriteFile(const char * path)
{
	SI_Error err;
	Logger.Log(LogLevel_DEBUG, LOGSENDER_CONFIG, "Saving '%s'", path);
	if ((err = ini.SaveFile(path)) != SI_OK) {
		throw ConfigException("Failed to save ini file", err);
	}
}

bool ConfigReader::FileExists(const char* path)
{
	ifstream infile(path);
	return infile.good();
}

bool ConfigReader::DirectoryExists(const char* path)
{
	struct stat info;

	if (stat(path, &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;
	else
		throw ConfigException("Config folder is no directory.");
}

void ConfigReader::CreateDirectoryStruct(const char * path)
{
	string s = string(path);
	size_t idx = 0;
	while ((idx = s.find("/", idx)) != string::npos) {
		string dir = s.substr(0, idx++);

		if (!DirectoryExists(dir.c_str())) {
			Logger.Log(LogLevel_DEBUG, LOGSENDER_CONFIG, "Creating folder '%s'", dir.c_str());
			int err = 0;
			if ((err = MKDIR(dir.c_str())) != 0) {
				throw ConfigException("Failed to create folder");
			}
		}
	}
}

void ConfigReader::SetBool(const char * section, const char * name, const char * comment, const bool & value)
{
	ini.SetBoolValue(section, name, value, comment);
}

bool ConfigReader::GetBool(const char * section, const char * name)
{
	return ini.GetBoolValue(section, name);
}

void ConfigReader::SetInt(const char * section, const char * name, const char * comment, const int32_t & value)
{
	ini.SetLongValue(section, name, (long)value, comment);
}

int32_t ConfigReader::GetInt(const char * section, const char * name)
{
	return (int32_t)ini.GetLongValue(section, name);
}

void ConfigReader::SetString(const char * section, const char * name, const char * comment, const string &value)
{
	string v = regex_replace(value, regex("\\\n"), "\\n");
	ini.SetValue(section, name, v.c_str(), comment);
}

const string ConfigReader::GetString(const char * section, const char * name)
{
	const char* c = ini.GetValue(section, name);
	return regex_replace(c, regex("\\\\n"), "\n");
}

vector<string> ConfigReader::GetAdditionalSections()
{
	CSimpleIniA::TNamesDepend sections;
	ini.GetAllSections(sections);

	//not the fastest solution, but keeps any SimpleIni-types out of the other classes
	vector<string> v;
	for (CSimpleIniA::Entry const &s : sections) v.push_back(string(s.pItem));

	return v;
}

bool ConfigReader::SectionExists(const char * section)
{
	return(ini.GetSection(section) != NULL);
}