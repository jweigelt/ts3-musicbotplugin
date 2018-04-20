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
#include "CmdPlayYouTube.h"
#include "MusicBot.h"

CmdPlayYouTube::CmdPlayYouTube() : RemoteCommand("playyoutube", "yt", "playyoutube", "yt <url>")
{
	yt = new YoutubeGrabber();
	using namespace placeholders;
	yt->SetGetOkCallback(bind(&CmdPlayYouTube::YT_OnGetOk, this, _1, _2));
	yt->SetGetFailedCallback(bind(&CmdPlayYouTube::YT_OnGetFailed, this, _1, _2));
}

CmdPlayYouTube::~CmdPlayYouTube()
{
	delete(yt);
}

bool CmdPlayYouTube::Execute(vector<string> const & message, Ts3User const & user)
{

	if (message.size() != 2) {
		return false;
	}

	if (yt->IsRunning())
	{
		SendToUser(user, onYoutubeRunning);
		return true;
	}

	string url = Utility::StripBB(message[1]);
	string videoId;
	size_t idStart = 0;
	size_t idEnd = 0;

	if (url.compare(0, strlen(HOST_YOUTUBE_COM), HOST_YOUTUBE_COM) == 0) {
		idStart = strlen(HOST_YOUTUBE_COM);
		idEnd = url.find("&", idStart);
	}
	else if (url.compare(0, strlen(HOST_YOUTU_BE), HOST_YOUTU_BE) == 0) {
		idStart = strlen(HOST_YOUTU_BE);
		idEnd = url.find("?", idStart);
	}
	else {
		SendToUser(user, onInvalidUrl);
		return true;
	}

	if (idEnd != string::npos) {
		videoId = url.substr(idStart, idEnd - idStart);
	}
	else {
		videoId = url.substr(idStart);
	}

	timestamp = 0;

	//Timestamp
	if ((idStart = url.find("t=", idEnd)) != string::npos) {
		idStart += 2;
		idEnd = url.find("&", idStart);
		string time = (idEnd == string::npos ? url.substr(idStart) : url.substr(idStart, idEnd - idStart));

		size_t start = 0;
		size_t idx = 0;

		try {
			for (char &c : time) {
				if (c == 'h' || c == 'm' || c == 's') {
					timestamp += stoi(time.substr(start, idx - start)) * 1000 * (c == 'm' || c == 'h' ? 60 : 1) * (c == 'h' ? 60 : 1);
					start = idx + 1;
				}
				idx++;
			}
		}
		catch (...) {
			SendToUser(user, onInvalidUrl);
			return true;
		}
	}

	yt->GetVideoInfoAsync(HOST_YOUTUBE_COM + videoId, user);

	return true;
}

void CmdPlayYouTube::SetDefaultConfig(ConfigReader * reader)
{
	reader->SetString(INI_SECTION_TEMPLATE, "ongetok", NULL, onYoutubeGetOk);
	reader->SetString(INI_SECTION_TEMPLATE, "ongetfailed", NULL, onYoutubeGetFailed);
	reader->SetString(INI_SECTION_TEMPLATE, "onrunning", NULL, onYoutubeRunning);
	reader->SetString(INI_SECTION_TEMPLATE, "oninvalidurl", NULL, onInvalidUrl);
}

void CmdPlayYouTube::ReadConfig(ConfigReader * reader)
{
	onYoutubeGetOk = reader->GetString(INI_SECTION_TEMPLATE, "ongetok");
	onYoutubeGetFailed = reader->GetString(INI_SECTION_TEMPLATE, "ongetfailed");
	onInvalidUrl = reader->GetString(INI_SECTION_TEMPLATE, "oninvalidurl");
	onYoutubeRunning = reader->GetString(INI_SECTION_TEMPLATE, "onrunning");
}

void CmdPlayYouTube::YT_OnGetOk(vector<YoutubeVideoInfo>& videos, const Ts3User & user)
{
	if (videos.size() > 0) {
		YoutubeVideoInfo info = videos[0];
		SendToUser(user, onYoutubeGetOk, info.title.c_str(), info.quality.c_str());
		RealtimePlayback* pb = GetBot()->GetPlayback();
		if (pb->IsPlaying()) pb->Stop();

		//TODO: fix
		while (pb->IsPlaying()) {}

		pb->PlayStream(info.url.c_str(), info.title, true);
		if (timestamp != 0 && pb->CanSeek(timestamp)) {
			pb->Seek(timestamp);
		}

	}
	else {
		SendToUser(user, onYoutubeGetFailed, "No videos found");
	}
}

void CmdPlayYouTube::YT_OnGetFailed(const exception & e, const Ts3User & user)
{
	SendToUser(user, onYoutubeGetFailed, e.what());
}