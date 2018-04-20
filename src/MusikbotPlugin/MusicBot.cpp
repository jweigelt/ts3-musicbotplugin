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

#include "MusicBot.h"

MusicBot::MusicBot(const TS3Functions ts3Api)
{
	this->ts3Api = ts3Api;
	
	config = new MainConfiguration();
	config->ReadConfig();

	Logger.SetFileName(config->GetLogFileName());
	Logger.SetMinLevelFile(config->GetLevelFile());
	Logger.SetMinLevelStdout(config->GetLevelStdout());

	playback = new RealtimePlayback(this);
	playback->RegisterVirtualDevice();
	playback->SetVolume(((double)config->GetAudioDefaultVolume()) / 100);
	playback->ConfigureNormalizer(config->GetAudioNormalizeEnable(), config->GetAudioNormalizeEngine(), config->GetAudioNormalizeEngineArgs());

	dispatcher = new RemoteCommandDispatcher(this);
	dispatcher->SetWriteGroups(config->GetPermissionAutoGroup());

	dispatcher->RegisterCommand((RemoteCommand*) new CmdPing());
	dispatcher->RegisterCommand((RemoteCommand*) new CmdPlayYouTube());
	dispatcher->RegisterCommand((RemoteCommand*) new CmdPlayRadio());
	dispatcher->RegisterCommand((RemoteCommand*) new CmdSeek());
	dispatcher->RegisterCommand((RemoteCommand*) new CmdStop());
	dispatcher->RegisterCommand((RemoteCommand*) new CmdSetVolume());

	animationProvider = new NameAnimationProvider(this);
	animationProvider->SetAnimation(config->GetAnimateTitle());
	animationProvider->SetNameFormat(config->GetAnimateTitleFormat());
	animationProvider->SetFlowFormat(config->GetAnimateFlowFormat());
	animationProvider->SetFlowDelay(config->GetAnimateFlowDelay());
	animationProvider->SetSwitchFormat(config->GetAnimateSwitchFormat());
	animationProvider->SetSwitchDelay(config->GetAnimateSwitchDelay());

	if (config->GetAutoConnectEnable()) {
		/*uint64_t sID = 0;
		unsigned int err = 0;
		if ((err = ts3Api.spawnNewServerConnectionHandler(0, &sID)) != TS3_OK) {
			printf("err1");
		}

		Logger.Log(LogLevel_INFO, LOGSENDER_CORE, "Attempting to connect...");

		if ((err = ts3Api.startConnection(sID, "7V3vd5EMwdT2LWyBR8a67EavAVLwJ8K3JBFntZeythHn4dRGUEX2cBLhRxAAhlE0NZFXFpdB97UiVVZH0Jc3EGAXxoVmlXVQFiUCNwKzIsGDVJXGcFK2BbDkYxZ0BkMkpUPWp8WmsxRS9BaUIydW1TU2JqekxzZ3AzZ3AreitvY3Bxa2VwL0syNytDaCs0QWtjVCtScE53PT0=", "localhost", 9987, config->GetAutoConnectNickName().c_str(), NULL, "", "")) != TS3_OK) {
			char* er = NULL;
			ts3Api.getErrorMessage(err, &er);
			printf(er);
		}

		ts3Api.requestServerGroupList(sID, NULL);
		ts3Api.openCaptureDevice(sID, "", "");*/
	}
	Logger.Log(LogLevel_DEBUG, LOGSENDER_CORE, "Init complete.");
}

MusicBot::~MusicBot()
{
	delete(playback);
	delete(dispatcher);
	delete(animationProvider);
	delete(config);
}

void MusicBot::Shutdown()
{
	playback->Shutdown();
	animationProvider->Shutdown();
}

RealtimePlayback * MusicBot::GetPlayback()
{
	return playback;
}

TS3Functions * MusicBot::GetApi()
{
	return &ts3Api;
}

RemoteCommandDispatcher * MusicBot::GetDispatcher()
{
	return dispatcher;
}

NameAnimationProvider * MusicBot::GetAnimationProvider()
{
	return animationProvider;
}

uint64_t MusicBot::GetServerConnectionHandlerID()
{
	return serverConnectionHandlerID;
}

void MusicBot::SetServerConnectionHandlerID(uint64_t id)
{
	serverConnectionHandlerID = id;
}