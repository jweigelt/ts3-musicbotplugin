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

#include <stdint.h>
#include <stdio.h>
#include <ts3_functions.h>


#ifdef _WIN32
#define _EXPORT __declspec(dllexport)
#else
#define _EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
	//Plugin API
	_EXPORT const char* ts3plugin_name();
	_EXPORT const char* ts3plugin_version();
	_EXPORT int ts3plugin_apiVersion();
	_EXPORT const char* ts3plugin_author();
	_EXPORT const char* ts3plugin_description();
	_EXPORT void ts3plugin_setFunctionPointers(const struct TS3Functions funcs);
	_EXPORT int ts3plugin_init();
	_EXPORT void ts3plugin_shutdown();
	
	//Callbacks
	_EXPORT int ts3plugin_onTextMessageEvent(uint64_t serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored);
	_EXPORT void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
	_EXPORT void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID);

}