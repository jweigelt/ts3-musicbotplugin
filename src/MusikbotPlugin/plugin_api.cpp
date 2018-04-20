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
#include "plugin_api.h"
#include "MusicBot.h"
#include "Definitions.h"

static MusicBot* musicBot;
static struct TS3Functions ts3funcs;

const char* ts3plugin_name()
{
	return PLUGIN_NAME;
}

const char * ts3plugin_version()
{
	return PLUGIN_VERSION;
}

int ts3plugin_apiVersion()
{
	return PLUGIN_API_VERSION;
}

const char* ts3plugin_author()
{
	return PLUGIN_AUTHOR;
}

const char* ts3plugin_description()
{
	return PLUGIN_DESCRIPTION;
}

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs)
{
	ts3funcs = funcs;
}

 int ts3plugin_init()
 {
	 musicBot = new MusicBot(ts3funcs);
	 //musicBot->Initialize();
	 return TS3_OK;
 }

 void ts3plugin_shutdown()
 {
	 if (musicBot != NULL) {
		 musicBot->Shutdown();
		 delete(musicBot);
	 }
 }

 int ts3plugin_onTextMessageEvent(uint64_t serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored)
 {
	 if (musicBot != NULL) musicBot->SetServerConnectionHandlerID(serverConnectionHandlerID);
	 musicBot->GetDispatcher()->OnTextMessageEvent(serverConnectionHandlerID, targetMode, toID, fromID,  fromName, fromUniqueIdentifier,  message,  ffIgnored);
	 return TS3_OK;
 }

 _EXPORT void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char * name, int type, int iconID, int saveDB)
 {
	 musicBot->GetDispatcher()->OnServerGroupListEvent(serverConnectionHandlerID, serverGroupID, name, type, iconID, saveDB);
 }

 _EXPORT void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID)
 {
	 //TODO: clean this up
	 if (musicBot != NULL) musicBot->SetServerConnectionHandlerID(serverConnectionHandlerID);
	 musicBot->GetDispatcher()->OnServerGroupListFinishedEvent(serverConnectionHandlerID);
 }
