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
#include <string>
#include "Logger.h"
#include "NameAnimationProvider.h"
#include "ConfigReader.h"
#include "Definitions.h"

using namespace std;

class MainConfiguration
{
public:
	MainConfiguration();
	~MainConfiguration();
	void ReadConfig();

	LogLevel GetLevelStdout();
	LogLevel GetLevelFile();
	string GetLogFileName();

	Animation GetAnimateTitle();
	string GetAnimateTitleFormat();
	int GetAnimateSwitchDelay();
	int GetAnimateFlowDelay();
	string GetAnimateSwitchFormat();
	string GetAnimateFlowFormat();

	bool GetPermissionAutoGroup();

	bool GetAutoConnectEnable();
	string GetAutoConnectHost();
	string GetAutoConnectNickName();

	bool GetAudioNormalizeEnable();
	string GetAudioNormalizeEngine();
	string GetAudioNormalizeEngineArgs();
	int GetAudioDefaultVolume();

private:
	ConfigReader *reader = NULL;

	LogLevel logLevelStdout = LogLevel_INFO;
	LogLevel logLevelFile = LogLevel_ERROR;
	string logFileName = "plugins/musikbot/log.txt";

	bool autoConnectEnable = true;
	string autoConnectHost = "localhost:9987";
	string autoConnectNickName = "Bot";

	Animation animateTitle = ANIMATION_SWITCH;
	string animateTitleFormat = "%s > %s";

	int animateSwitchDelay = 5000;
	int animateFlowDelay = 400;
	string animateFlowFormat = "%s --- ";
	string animateSwitchFormat = "%s --- ";

	bool permissionAutoGroup = true;

	bool audioNormalizeEnable = true;
	string audioNormalizeEngine = "dynaudnorm";
	string audioNormalizeEngineArgs = "f=100";
	int audioDefaultVolume = 100;

	void SetDefaultConfig();
};

