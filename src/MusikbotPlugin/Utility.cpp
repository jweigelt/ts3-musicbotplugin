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
#include "Utility.h"

Utility::Utility()
{
}

Utility::~Utility()
{
}

vector<string> Utility::SplitString(string const &str, string const &split)
{
	vector<string> res = vector<string>();
	size_t idx = 0;
	size_t lastIdx = 0;

	while ((idx = str.find(split, idx)) != string::npos)
	{
		res.push_back(str.substr(lastIdx, idx- lastIdx));
		lastIdx = ++idx;
	}

	res.push_back(str.substr(lastIdx));
	return res;
}

string Utility::StripBB(const string & str)
{
	string r = str;
	r = regex_replace(r, regex("\\[URL\\]"), "");
	r = regex_replace(r, regex("\\[/URL\\]"), "");
	return r;
}

string Utility::FormatString(const string fmt, ...)
{
	va_list args;
	size_t szr = 0;
	char* msg = NULL;

	va_start(args, fmt);
	szr = vsnprintf(NULL, 0, fmt.c_str(), args) + 1;
	msg = new char[szr + 1];
	_vsprintf_s(msg, szr, fmt.c_str(), args);
	va_end(args);

	string s = string(msg);
	delete[](msg);
	return s;
}