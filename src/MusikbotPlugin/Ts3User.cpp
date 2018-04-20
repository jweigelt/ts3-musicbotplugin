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
#include "Ts3User.h"

Ts3User::Ts3User(string userName, string uuqId, anyID id, anyID dbId, uint64_t serverId)
{
	this->userName = userName;
	this->uuqId = uuqId;
	this->id = id;
	this->serverId = serverId;
}

void Ts3User::SendPrivateMessage(const char * msg, TS3Functions * api) const
{
	api->requestSendPrivateTextMsg(serverId, msg, id, NULL);
}

vector<uint16_t> Ts3User::GetServerGroups(TS3Functions * api) const
{
	vector<uint16_t> res = vector<uint16_t>();
	char* sgl;
	if (api->getClientVariableAsString(serverId, id, CLIENT_SERVERGROUPS, &sgl) == TS3_OK) {
		vector<string> groups = Utility::SplitString(sgl, ",");
		for (string const &s : groups) {
			res.push_back(stoi(s));
		}
	}
	else {
		Logger.Log(LogLevel_ERROR, LOGSENDER_COMMAND, "Failed to fetch client grouplist. Blocking command access.");
	}
	return res;
}

Ts3User::~Ts3User()
{
}


