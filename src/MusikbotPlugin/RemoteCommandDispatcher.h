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
#include <vector>
#include <stdint.h>
#include "RemoteCommand.h"
#include "Ts3User.h"
#include "ts3_functions.h"

#define COMMAND_SPLITKEY " "

using namespace std;

class RemoteCommand;

struct ServerGroup {
	vector<string> permissions = vector<string>();
	bool grantAll = false;
	string name;
	uint64_t id;
	ServerGroup(string n, uint64_t i) : name(n), id(i) {}
};

class RemoteCommandDispatcher
{

public:
	RemoteCommandDispatcher(MusicBot* bot);
	~RemoteCommandDispatcher();

	void OnTextMessageEvent(uint64_t serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored);
	void OnServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
	void OnServerGroupListFinishedEvent(uint64 serverConnectionHandlerID);
	
	void RegisterCommand(RemoteCommand* command);
	void SetWriteGroups(bool writeGroups);

private:
	vector <ServerGroup*> serverGroups = vector<ServerGroup*>();
	vector<RemoteCommand*> commands = vector<RemoteCommand*>();

	bool permissionWriteDefaults = false;
	bool groupListComplete = false;
	bool writeGroups = true;

	ServerGroup* GetServerGroupById(uint64_t id);

	bool HasPermission(Ts3User const &user, RemoteCommand const &command);
	MusicBot* bot;

	ConfigReader* permissionsReader;
};