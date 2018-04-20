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
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <mutex>
#include <teamlog\logtypes.h>
using namespace std;

/*
enum LogLevel {
	LogLevel_VERBOSE = 0,
	LogLevel_INFO = 1,
	LogLevel_WARNING = 2,
	LogLevel_ERROR = 3,
	LOGLEVEL_CRITICAL = 4
};
*/

#define LOGSENDER_YOUTUBE	"[YT]	"
#define LOGSENDER_COMMAND	"[CMD]	"
#define LOGSENDER_AUDIO		"[AUDIO]"
#define LOGSENDER_CONFIG	"[CFG]	"
#define LOGSENDER_CORE		"[CORE]	"



static const char* LOG_LEVELS[] =
{
	"CRIT  | ",
	"ERROR | ",
	"WARN  | ",
	"DEBUG | ",
	"INFO  | ",
	"DEVEL | "
};



class _Logger
{
public:
	_Logger();
	~_Logger();
	void Log(LogLevel level, const char* sender, const char* msg, ...);
	void SetMinLevelStdout(LogLevel level);
	void SetMinLevelFile(LogLevel level);
	void SetFileName(const string &fileName);

private:
	void LogToFile(const string &s);
	LogLevel minLevelStdout = LogLevel_INFO;
	LogLevel minLevelFile = LogLevel_WARNING;
	mutex mtx;
	string logFile = "C:\\Users\\Jan\\Desktop\\log.txt";
};

static _Logger Logger;