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
#include "CmdPlayRadio.h"
#include "MusicBot.h"

CmdPlayRadio::CmdPlayRadio() : RemoteCommand("playradio", "radio", "playradio", "radio -l (list stations) or radio <station> (play station)")
{ }

CmdPlayRadio::~CmdPlayRadio()
{ }

bool CmdPlayRadio::Execute(vector<string> const & message, Ts3User const & user)
{
	bool found = false;
	if (message.size() != 2) return false;

	if (message[1] == "-l") {
		string stationList;
		for (RadioStation const &sta : storedStations) {
			stationList += sta.alias + " - " + sta.name + " - " + sta.location + "\n";
		}

		SendToUser(user, onStationList, stationList.c_str());
		return true;
	}

	for (RadioStation const &s : storedStations) {
		if (s.alias == message[1]) {
			SendToUser(user, onRadioPlay, s.name.c_str());
			GetBot()->GetPlayback()->PlayStream(s.url, s.name, false);
			found = true;
			continue;
		}
	}

	if (!found) {
		SendToUser(user, onStationInvalid);
	}

	return true;
}

void CmdPlayRadio::SetDefaultConfig(ConfigReader * reader)
{
	reader->SetString(INI_SECTION_TEMPLATE, "onplay", NULL, onRadioPlay);
	reader->SetString(INI_SECTION_TEMPLATE, "onstationinvalid", NULL, onStationInvalid);
	reader->SetString(INI_SECTION_TEMPLATE, "onstationlist", NULL, onStationList);

	RegisterStation(reader, "1live", "WDR 1Live", "http://wdr-1live-live.icecast.wdr.de/wdr/1live/live/mp3/128/stream.mp3", "DE-Koeln");
	RegisterStation(reader, "wdr2", "WDR 2", "http://wdr-wdr2-rheinland.icecast.wdr.de/wdr/wdr2/rheinland/mp3/128/stream.mp3", "DE-Koeln");
	RegisterStation(reader, "wdr3", "WDR 3", "http://wdr-wdr3-live.icecast.wdr.de/wdr/wdr3/live/mp3/256/stream.mp3", "DE-Koeln");
	RegisterStation(reader, "wdr4", "WDR 4", "http://wdr-wdr4-live.icecast.wdr.de/wdr/wdr4/live/mp3/128/stream.mp3", "DE-Koeln");
	RegisterStation(reader, "wdr5", "WDR 5", "http://wdr-wdr5-live.icecast.wdr.de/wdr/wdr5/live/mp3/128/stream.mp3", "DE-Koeln");

	RegisterStation(reader, "hr3", "HR 3", "https://hr-hr3-live.sslcast.addradio.de/hr/hr3/live/mp3/128/stream.mp3", "DE-Frankfurt");
}

void CmdPlayRadio::ReadConfig(ConfigReader * reader)
{
	onRadioPlay = reader->GetString(INI_SECTION_TEMPLATE, "onplay");
	onStationInvalid = reader->GetString(INI_SECTION_TEMPLATE, "onstationinvalid");
	onStationList = reader->GetString(INI_SECTION_TEMPLATE, "onstationlist");
	vector<string> sections = reader->GetAdditionalSections();

	for (string const &s : sections) {
		if (s.find(STATION_MAGICSTR) == 0) {
			RadioStation sta;
			sta.alias = s.substr(strlen(STATION_MAGICSTR));
			sta.name = reader->GetString(s.c_str(), "name");
			sta.url = reader->GetString(s.c_str(), "url");
			sta.location = reader->GetString(s.c_str(), "location");
			storedStations.push_back(sta);
		}
	}
}

void CmdPlayRadio::RegisterStation(ConfigReader * reader, string const & alias, string const & name, string const & url, string const &location)
{
	string s = string(STATION_MAGICSTR) + alias;
	const char* sec = s.c_str();

	reader->SetString(sec, "name", NULL, name);
	reader->SetString(sec, "url", NULL, url);
	reader->SetString(sec, "location", NULL, location);
}