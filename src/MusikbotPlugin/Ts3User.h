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
#include <string>
#include <vector>
#include <ts3_functions.h>
#include "Utility.h"
#include "Logger.h"
#include "Definitions.h"
using namespace std;

class Ts3User
{
public:
	Ts3User(string userName, string uuqId, anyID id, anyID dbId, uint64_t serverId);
	~Ts3User();

	void SendPrivateMessage(const char* msg, TS3Functions* api) const;
	vector<uint16_t>GetServerGroups(TS3Functions * api) const;


private:
	string userName;
	string uuqId;
	anyID id;
	uint64_t serverId;
};