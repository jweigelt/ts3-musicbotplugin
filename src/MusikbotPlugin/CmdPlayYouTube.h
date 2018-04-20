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
#include "RemoteCommand.h"
#include "YoutubeGrabber.h"
#include "Utility.h"

#define HOST_YOUTUBE_COM  "https://www.youtube.com/watch?v="
#define HOST_YOUTU_BE "https://youtu.be/"

class CmdPlayYouTube : RemoteCommand
{
public:
	CmdPlayYouTube();
	~CmdPlayYouTube();
	bool Execute(vector<string> const &message, Ts3User const &user) override;
	void SetDefaultConfig(ConfigReader* reader) override;
	void ReadConfig(ConfigReader* reader) override;
	void YT_OnGetOk(vector<YoutubeVideoInfo> &videos, const Ts3User &user);
	void YT_OnGetFailed(const exception &e, const Ts3User &user);
private:
	YoutubeGrabber *yt = NULL;
	string onYoutubeGetOk = "Playing '%s' (%s)";
	string onYoutubeGetFailed = "Couldn't retrieve video. (%s)";
	string onYoutubeRunning = "YouTube-Parser is running. Please try again in a moment.";
	string onInvalidUrl = "Invalid URL. Only youtube.com/watch?... and youtu.be/... URLs are allowed.";
	uint64_t timestamp = 0;
};