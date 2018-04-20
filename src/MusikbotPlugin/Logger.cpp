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
#include "Logger.h"
_Logger::_Logger()
{
}

_Logger::~_Logger()
{
}

void _Logger::Log(LogLevel level, const char * sender, const char * msg, ...)
{
	if (minLevelStdout < level) {
		lock_guard <mutex> lg(mtx);
		{
			printf(LOG_LEVELS[level]);
			va_list args;
			va_start(args, msg);
			vprintf(msg, args);
			va_end(args);
			printf("\n");
		}
	}

	if (minLevelFile < level) {
		//TODO: fix me
		LogToFile(msg);
	}
}

void _Logger::SetMinLevelStdout(LogLevel level)
{
	minLevelStdout = level;
}

void _Logger::SetMinLevelFile(LogLevel level)
{
	minLevelFile = level;
}

void _Logger::SetFileName(const string & fileName)
{
	logFile = fileName;
}

void _Logger::LogToFile(const string & s)
{
	ofstream f;
	f.open(logFile, f.app);
	f << s.c_str() << "\n";
	f.close();
}