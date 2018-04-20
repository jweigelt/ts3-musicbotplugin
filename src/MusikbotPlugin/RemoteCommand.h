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
class RemoteCommand;

#include <string>
#include "ConfigReader.h"
#include "Ts3User.h"
#include "Definitions.h"

using namespace std;

class MusicBot;

class RemoteCommand
{
public:
	RemoteCommand(string name, string alias, string permission, string usage);
	~RemoteCommand();

	virtual bool Execute(vector<string> const &message, Ts3User const &user);
	virtual void SetDefaultConfig(ConfigReader* reader);
	virtual void ReadConfig(ConfigReader* reader);

	void ReadConfig();
	string getCommandAlias() const;
	string GetPermission() const;
	void SendToUser(const Ts3User &user, const string message, ...);
	void SendSyntaxErr(const Ts3User &user);

	MusicBot* GetBot();
	void SetBot(MusicBot* bot);

private:
	string commandName;
	string commandAlias;
	string commandPermission;
	string commandUsage;
	MusicBot* bot = NULL;
};