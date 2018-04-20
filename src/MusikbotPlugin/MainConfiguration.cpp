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
#include "MainConfiguration.h"

MainConfiguration::MainConfiguration()
{
	reader = new ConfigReader();
	SetDefaultConfig();
	reader->ReadFile(INI_FILE_MAIN);
}

MainConfiguration::~MainConfiguration()
{
	delete(reader);
}

void MainConfiguration::ReadConfig()
{
	logLevelStdout = (LogLevel)reader->GetInt(INI_SECTION_LOGGER, "level_console");
	logLevelFile = (LogLevel)reader->GetInt(INI_SECTION_LOGGER, "level_file");
	logFileName = reader->GetString(INI_SECTION_LOGGER, "logfile");

	autoConnectEnable = reader->GetBool(INI_SECTION_AUTOCONNECT, "enable");
	autoConnectHost = reader->GetString(INI_SECTION_AUTOCONNECT, "host");
	autoConnectNickName = reader->GetString(INI_SECTION_AUTOCONNECT, "nick");

	animateTitle = (Animation)reader->GetInt(INI_SECTION_ANIMATION, "animation");
	animateTitleFormat = reader->GetString(INI_SECTION_ANIMATION, "format");

	animateSwitchDelay = reader->GetInt(INI_SECTION_ANIMATION, "switch_delay");
	animateSwitchFormat = reader->GetString(INI_SECTION_ANIMATION, "switch_format");

	animateFlowDelay = reader->GetInt(INI_SECTION_ANIMATION, "scroll_delay");
	animateFlowFormat = reader->GetString(INI_SECTION_ANIMATION, "scroll_format");

	permissionAutoGroup = reader->GetBool(INI_SECTION_PERMISSIONS, "auto_group");

	audioNormalizeEngineArgs = reader->GetString(INI_SECTION_AUDIO, "normalize_engineargs");
	audioNormalizeEngine = reader->GetString(INI_SECTION_AUDIO, "normalize_engine");
	audioNormalizeEnable = reader->GetBool(INI_SECTION_AUDIO, "normalize_enable");
	audioDefaultVolume = reader->GetInt(INI_SECTION_AUDIO, "default_volume");

	//Prevent spamming
	if (animateFlowDelay < 100) {
		animateFlowDelay = 100;
		Logger.Log(LogLevel_ERROR, LOGSENDER_CONFIG, "scroll_delay too small. Setting it to 100ms.");
	}

	if (animateSwitchDelay < 100) {
		animateSwitchDelay = 100;
		Logger.Log(LogLevel_ERROR, LOGSENDER_CONFIG, "switch_delay too small. Setting it to 100ms.");
	}

	//A bit of additional validation
	if (audioDefaultVolume < 0 || audioDefaultVolume > 100) {
		audioDefaultVolume = 100;
		Logger.Log(LogLevel_ERROR, LOGSENDER_CONFIG, "Invalid default volume. [0-100]. Setting volume to 100.");
	}

	if (audioNormalizeEngine != "dynaudnorm" && audioNormalizeEngine != "loudnorm") {
		audioNormalizeEngine = "dynaudnorm";
		Logger.Log(LogLevel_ERROR, LOGSENDER_CONFIG, "Invalid audio normalization engine. Using dynaudnorm.");
	}
}

void MainConfiguration::SetDefaultConfig()
{
	reader->SetInt(INI_SECTION_LOGGER, "level_console", NULL, (int)logLevelStdout);
	reader->SetInt(INI_SECTION_LOGGER, "level_file", NULL, (int)logLevelFile);
	reader->SetString(INI_SECTION_LOGGER, "logfile", NULL, logFileName);

	reader->SetBool(INI_SECTION_AUTOCONNECT, "enable", NULL, autoConnectEnable);
	reader->SetString(INI_SECTION_AUTOCONNECT, "host", NULL, autoConnectHost);
	reader->SetString(INI_SECTION_AUTOCONNECT, "nick", NULL, autoConnectNickName);

	reader->SetInt(INI_SECTION_ANIMATION, "animation", NULL, (int)animateTitle);
	reader->SetString(INI_SECTION_ANIMATION, "format", NULL, animateTitleFormat);

	reader->SetInt(INI_SECTION_ANIMATION, "switch_delay", NULL, animateSwitchDelay);
	reader->SetString(INI_SECTION_ANIMATION, "switch_format", NULL, animateSwitchFormat);

	reader->SetInt(INI_SECTION_ANIMATION, "scroll_delay", NULL, animateFlowDelay);
	reader->SetString(INI_SECTION_ANIMATION, "scroll_format", NULL, animateFlowFormat);

	reader->SetBool(INI_SECTION_PERMISSIONS, "auto_group", NULL, permissionAutoGroup);

	reader->SetString(INI_SECTION_AUDIO, "normalize_engineargs", NULL, audioNormalizeEngineArgs);
	reader->SetString(INI_SECTION_AUDIO, "normalize_engine", NULL, audioNormalizeEngine);
	reader->SetBool(INI_SECTION_AUDIO, "normalize_enable", NULL, audioNormalizeEnable);
	reader->SetInt(INI_SECTION_AUDIO, "default_volume", NULL, audioDefaultVolume);
}

LogLevel MainConfiguration::GetLevelStdout()
{
	return (LogLevel)logLevelStdout;
}

LogLevel MainConfiguration::GetLevelFile()
{
	return (LogLevel)logLevelFile;
}

string MainConfiguration::GetLogFileName()
{
	return logFileName;
}

Animation MainConfiguration::GetAnimateTitle()
{
	return animateTitle;
}

string MainConfiguration::GetAnimateTitleFormat()
{
	return animateTitleFormat;
}

int MainConfiguration::GetAnimateSwitchDelay()
{
	return animateSwitchDelay;
}

int MainConfiguration::GetAnimateFlowDelay()
{
	return animateFlowDelay;
}

string MainConfiguration::GetAnimateSwitchFormat()
{
	return animateSwitchFormat;
}

string MainConfiguration::GetAnimateFlowFormat()
{
	return animateFlowFormat;
}

bool MainConfiguration::GetPermissionAutoGroup()
{
	return permissionAutoGroup;
}

bool MainConfiguration::GetAutoConnectEnable()
{
	return autoConnectEnable;
}

string MainConfiguration::GetAutoConnectHost()
{
	return autoConnectHost;
}

string MainConfiguration::GetAutoConnectNickName()
{
	return autoConnectNickName;
}

bool MainConfiguration::GetAudioNormalizeEnable()
{
	return audioNormalizeEnable;
}

string MainConfiguration::GetAudioNormalizeEngine()
{
	return audioNormalizeEngine;
}

string MainConfiguration::GetAudioNormalizeEngineArgs()
{
	return audioNormalizeEngineArgs;
}

int MainConfiguration::GetAudioDefaultVolume()
{
	return audioDefaultVolume;
}