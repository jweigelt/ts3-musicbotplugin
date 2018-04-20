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
#include <iostream>
#include "RemoteCommand.h"
#include "RealtimePlayback.h"
#include "RemoteCommandDispatcher.h"

#include "CmdPing.h"
#include "CmdPlayYouTube.h"
#include "CmdPlayRadio.h"
#include "CmdStop.h"
#include "CmdSeek.h"
#include "CmdSetVolume.h"

#include "NameAnimationProvider.h"
#include "MainConfiguration.h"

#include "ts3_functions.h"

using namespace std;

class MusicBot
{
public:
	MusicBot(const struct TS3Functions ts3Api);
	~MusicBot();

	void Shutdown();

	RealtimePlayback* GetPlayback();
	TS3Functions* GetApi();
	RemoteCommandDispatcher* GetDispatcher();
	NameAnimationProvider* GetAnimationProvider();
	uint64_t GetServerConnectionHandlerID();
	void SetServerConnectionHandlerID(uint64_t id);

private:
	TS3Functions ts3Api;
	uint64_t serverConnectionHandlerID = 0;
	RealtimePlayback* playback = NULL;
	RemoteCommandDispatcher* dispatcher = NULL;
	NameAnimationProvider* animationProvider = NULL;
	MainConfiguration* config = NULL;
};