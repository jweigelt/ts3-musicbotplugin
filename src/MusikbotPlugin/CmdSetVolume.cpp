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
#include "CmdSetVolume.h"
#include "MusicBot.h"

CmdSetVolume::CmdSetVolume() :RemoteCommand("setvolume", "vol", "setvolume", "vol <volume>")
{

}


CmdSetVolume::~CmdSetVolume()
{
}

bool CmdSetVolume::Execute(vector<string> const & message, Ts3User const & user)
{
	if (message.size() != 2) {
		return false;
	}

	int volume = 0;

	try {
		volume = stoi(message[1]);
	}
	catch (exception e) {
		SendToUser(user, onInavlidVolume);
		return true;
	}

	if (volume < 0 || volume > 100) {
		SendToUser(user, onInavlidVolume);
		return true;
	}
	
	double v = (double)volume / 100.0;
	GetBot()->GetPlayback()->SetVolume(v);
	SendToUser(user, onSetVolume, volume);
	return true;
}

void CmdSetVolume::SetDefaultConfig(ConfigReader * reader)
{
	reader->SetString(INI_SECTION_TEMPLATE, "onsetvolume", NULL, onSetVolume);
	reader->SetString(INI_SECTION_TEMPLATE, "oninvalidvolume", NULL, onInavlidVolume);
}

void CmdSetVolume::ReadConfig(ConfigReader * reader)
{
}
