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
#include "CmdSeek.h"
#include "MusicBot.h"

CmdSeek::CmdSeek() :RemoteCommand("seek", "seek", "seek", "seek")
{
}

CmdSeek::~CmdSeek()
{
}

bool CmdSeek::Execute(vector<string> const & message, Ts3User const &user)
{
	if (message.size() != 2) return false;

	vector<string> seekPos = Utility::SplitString(message[1], ":");
	if (seekPos.size() < 1 || seekPos.size() > 3) return false;

	uint64_t ms = 0;


	for (uint8_t i = 0; i < seekPos.size(); i++) {
		try {
			ms += stoi(seekPos[i]) * 1000 * (uint64_t)pow(60, seekPos.size() - i - 1);
		}
		catch (exception e) {
			SendToUser(user, onInvalidTimestamp);
			return true;
		}
	}

	RealtimePlayback* pb = GetBot()->GetPlayback();

	if (!pb->IsPlaying()) {
		SendToUser(user, onNotPlaying);
		return true;
	}

	if (!pb->CanSeek(ms)) {
		SendToUser(user, onNotSeekable);
		return true;
	}

	pb->Seek(ms);

	ms /= 1000;
	uint8_t h = (uint8_t) (ms / (60 * 60));
	uint8_t m = (uint8_t) ((ms % (60 * 60)) / 60);
	uint8_t s = (uint8_t) (ms % 60);

	SendToUser(user, onSeek, h, m, s);

	return true;
}

void CmdSeek::SetDefaultConfig(ConfigReader * reader)
{
	reader->SetString(INI_SECTION_TEMPLATE, "onseek", NULL, onSeek);
	reader->SetString(INI_SECTION_TEMPLATE, "onnotplaying", NULL, onNotPlaying);
	reader->SetString(INI_SECTION_TEMPLATE, "onnotseekable", NULL, onNotSeekable);
	reader->SetString(INI_SECTION_TEMPLATE, "oninvalidtimestamp", NULL, onInvalidTimestamp);
}

void CmdSeek::ReadConfig(ConfigReader * reader)
{
	onSeek = reader->GetString(INI_SECTION_TEMPLATE, "onseek");
	onNotPlaying = reader->GetString(INI_SECTION_TEMPLATE, "onnotplaying");
	onNotSeekable = reader->GetString(INI_SECTION_TEMPLATE, "onnotseekable");
	onInvalidTimestamp = reader->GetString(INI_SECTION_TEMPLATE, "oninvalidtimestamp");
}