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
#include "RemoteCommandDispatcher.h"
#include "MusicBot.h"

RemoteCommandDispatcher::RemoteCommandDispatcher(MusicBot * bot)
{
	this->bot = bot;
	permissionsReader = new ConfigReader();
	permissionsReader->ReadFile(INI_FILE_PERMISSIONS);
}

RemoteCommandDispatcher::~RemoteCommandDispatcher()
{
	for (RemoteCommand *c : commands) delete(c);
	commands.clear();

	for (ServerGroup* g : serverGroups) delete(g);
	serverGroups.clear();

	delete(permissionsReader);
} 

void RemoteCommandDispatcher::OnTextMessageEvent(uint64_t serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char * fromName, const char * fromUniqueIdentifier, const char * message, int ffIgnored)
{
	if (targetMode != TextMessageTarget_CLIENT) {
		return;
	}

	vector<string> params = Utility::SplitString(string(message), COMMAND_SPLITKEY);
	for (RemoteCommand *c : commands) {
		if (c->getCommandAlias().compare(params.at(0)) == 0) {
			Ts3User user = Ts3User(string(fromName), string(fromUniqueIdentifier), fromID, fromID, serverConnectionHandlerID);
			if (HasPermission(user, *c)) {
				if (c->Execute(params, user)) {
					Logger.Log(LogLevel_INFO, LOGSENDER_COMMAND, "%s: running command %s", fromName, params[0].c_str());
				}
				else {
					Logger.Log(LogLevel_INFO, LOGSENDER_COMMAND, "%s: syntax error running %s", fromName, params[0].c_str());
				}
			}
			else {
				Logger.Log(LogLevel_INFO, LOGSENDER_COMMAND, "%s: access to %s denied", fromName, params[0].c_str());
			}
			return;
		}
	}
	Logger.Log(LogLevel_INFO, LOGSENDER_COMMAND, "'%s' sent unknown command '%s'", fromName, params[0].c_str());
}

void RemoteCommandDispatcher::OnServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char * name, int type, int iconID, int saveDB)
{
	ServerGroup *g = new ServerGroup(name, serverGroupID);

	if (permissionsReader->SectionExists(g->name.c_str())) {
		string perms = permissionsReader->GetString(g->name.c_str(), INI_KEY_GROUP_PERMISSIONS);
		g->permissions = Utility::SplitString(perms, ",");
	}
	else if (writeGroups) {
		Logger.Log(LogLevel_INFO, LOGSENDER_CONFIG, "Generating default permissions for servergroup %s", g->name.c_str());
		permissionsReader->SetString(g->name.c_str(), INI_KEY_GROUP_PERMISSIONS, NULL, string());

#ifdef PERMISSIONS_GRANT_ALL
		permissionsReader->SetBool(g->name.c_str(), INI_KEY_GROUP_GRANT_ALL, NULL, (g->name == PERMISSIONS_GRANT_ALL));
#else
		permissionsReader->SetBool(g->name.c_str(), INI_KEY_GROUP_GRANT_ALL, NULL, false);
#endif
		permissionWriteDefaults = true;
	}

	if (permissionsReader->GetBool(g->name.c_str(), INI_KEY_GROUP_GRANT_ALL)) {
		g->grantAll = true;
	}

	serverGroups.push_back(g);
}

void RemoteCommandDispatcher::OnServerGroupListFinishedEvent(uint64 serverConnectionHandlerID)
{
	groupListComplete = true;

	if (permissionWriteDefaults) {
		permissionsReader->WriteFile(INI_FILE_PERMISSIONS);
	}
}

void RemoteCommandDispatcher::RegisterCommand(RemoteCommand *command)
{
	command->ReadConfig();
	command->SetBot(bot);
	commands.push_back(command);
}

void RemoteCommandDispatcher::SetWriteGroups(bool writeGroups)
{
	this->writeGroups = writeGroups;
}

ServerGroup * RemoteCommandDispatcher::GetServerGroupById(uint64_t id)
{
	for (ServerGroup* g : serverGroups) {
		if (g->id == id) return g;
	}
	return nullptr;
}

bool RemoteCommandDispatcher::HasPermission(Ts3User const & user, RemoteCommand const & command)
{
	vector<uint16_t> serverGroups = user.GetServerGroups(bot->GetApi());
	ServerGroup* g;
	for (uint16_t i : serverGroups) {
		if ((g = GetServerGroupById(i)) != nullptr) {
			if (g->grantAll) return true;
			for (string const &p : g->permissions) {
				if (p == command.GetPermission()) return true;
			}
		}
	}
	return false;
}