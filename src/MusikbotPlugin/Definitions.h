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
#include <teamspeak/public_definitions.h>
#include <teamspeak/public_rare_definitions.h>

/**
 * Config
 **/

//Commands
#define INI_CMD_SUBFOLDER "plugins/musikbot/commands/"
#define INI_SECTION_GENERIC "generic"
#define INI_SECTION_TEMPLATE "template"
#define INI_KEY_ALIAS "alias"
#define INI_KEY_PERMISSION "permission"
#define INI_KEY_USAGE "usage"
#define STATION_MAGICSTR "station::"

//Permissions
#define INI_FILE_PERMISSIONS "plugins/musikbot/permissions.ini"
#define INI_KEY_GROUP_PERMISSIONS "permissions"
#define INI_KEY_GROUP_GRANT_ALL	"grant_all"
#define PERMISSIONS_GRANT_ALL "Server Admin"

//Main
#define INI_FILE_MAIN "plugins/musikbot/main.ini"
#define INI_SECTION_LOGGER "logger"
#define INI_SECTION_AUTOCONNECT "autoconnect"
#define INI_SECTION_ANIMATION "animation"
#define INI_SECTION_PERMISSIONS "permissions"
#define INI_SECTION_AUDIO "audio"

/**
 * Ts3
 **/
#define PLUGIN_API_VERSION		20
#define PLUGIN_NAME				"Ts3 Musikbot"
#define PLUGIN_VERSION			"2.0"
#define PLUGIN_AUTHOR			"JW"
#define PLUGIN_DESCRIPTION		"Ein simpler Musikbot."
#define LOG_CHANNEL				"Plugin"

#define TS3_OK		0
#define TS3_ERROR	1

#define TS_MAX_NAME_LEN 30