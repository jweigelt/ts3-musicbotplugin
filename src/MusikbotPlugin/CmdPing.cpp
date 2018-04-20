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
#include "CmdPing.h"
#include "MusicBot.h"

CmdPing::CmdPing() : RemoteCommand("ping", "ping", "ping", "ping")
{
}

CmdPing::~CmdPing()
{
}

bool CmdPing::Execute(vector<string> const & message, Ts3User const &user)
{
	user.SendPrivateMessage(onPing.c_str(), GetBot()->GetApi());
	return true;
}

void CmdPing::SetDefaultConfig(ConfigReader* reader)
{
	reader->SetString(INI_SECTION_TEMPLATE, INI_KEY_ONPING, NULL, "onping");
}

void CmdPing::ReadConfig(ConfigReader * reader)
{
	onPing = reader->GetString(INI_SECTION_TEMPLATE, "onping");
}
