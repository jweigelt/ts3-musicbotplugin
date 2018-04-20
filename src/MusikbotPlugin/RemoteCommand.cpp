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
#include "RemoteCommand.h"
#include "MusicBot.h"

RemoteCommand::RemoteCommand(string name, string alias, string permission, string usage)
{
	commandName = name;
	commandAlias = alias;
	commandPermission = permission;
	commandUsage = usage;
}

RemoteCommand::~RemoteCommand()
{
}

bool RemoteCommand::Execute(vector<string> const &message, Ts3User const &user)
{
	return true;
}

void RemoteCommand::SetDefaultConfig(ConfigReader * reader)
{
}

void RemoteCommand::ReadConfig(ConfigReader * reader)
{
}

void RemoteCommand::ReadConfig()
{
	ConfigReader r;

	r.SetString(INI_SECTION_GENERIC, INI_KEY_ALIAS, NULL, commandAlias);
	r.SetString(INI_SECTION_GENERIC, INI_KEY_PERMISSION, NULL, commandPermission);
	r.SetString(INI_SECTION_GENERIC, INI_KEY_USAGE, NULL, commandUsage);

	SetDefaultConfig(&r);

	r.ReadFile((INI_CMD_SUBFOLDER + commandName + ".ini").c_str());

	commandAlias = r.GetString(INI_SECTION_GENERIC, INI_KEY_ALIAS);
	commandPermission = r.GetString(INI_SECTION_GENERIC, INI_KEY_PERMISSION);
	commandUsage = r.GetString(INI_SECTION_GENERIC, INI_KEY_USAGE);

	ReadConfig(&r);
	//delete(reader);
}

string RemoteCommand::getCommandAlias() const
{
	return commandAlias;
}

string RemoteCommand::GetPermission() const
{
	return commandPermission;
}

void RemoteCommand::SendToUser(const Ts3User & user, const string message, ...)
{
	va_list args;
	size_t szr = 0;
	char* msg = NULL;

	va_start(args, message);
	szr = vsnprintf(NULL, 0, message.c_str(), args) + 1;
	msg = new char[szr + 1];
	_vsprintf_s(msg, szr, message.c_str(), args);
	va_end(args);

	user.SendPrivateMessage(msg, GetBot()->GetApi());
	delete[](msg);
}

void RemoteCommand::SendSyntaxErr(const Ts3User & user)
{
	SendToUser(user, commandUsage);
}

MusicBot * RemoteCommand::GetBot()
{
	return bot;
}

void RemoteCommand::SetBot(MusicBot * bot)
{
	this->bot = bot;
}