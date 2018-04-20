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
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

class MusicBot;

enum Animation {
	ANIMATION_NONE,
	ANIMATION_CUTOFF,
	ANIMATION_SWITCH,
	ANIMATION_FLOW
};

using namespace std;
class NameAnimationProvider
{
public:
	NameAnimationProvider(MusicBot* bot);
	~NameAnimationProvider();
	void UpdatePlaybackTitle(string const &s);
	void ResetTitle();

	void SetNickName(const string& nickName);
	void SetNameFormat(const string& nameFormat);
	void SetSwitchFormat(const string& switchFormat);
	void SetFlowFormat(const string& flowFormat);
	void SetAnimation(Animation animation);

	void SetFlowDelay(uint32_t flowDelay);
	void SetSwitchDelay(uint32_t switchDelay);

	void Shutdown();
private:
	MusicBot* bot;
	uint32_t flowDelay = 100;
	uint32_t switchDelay = 5000;
	uint32_t animationIndex = 0;
	uint8_t titleLength = 0;

	Animation animation = ANIMATION_SWITCH;

	thread* animationThread = NULL;
	condition_variable cv;
	mutex mtx;

	string playbackTitle = "";
	string nickName = "test";
	string nameFormat = "%s| %s";
	string flowFormat = "%s --- ";
	string switchFormat = "%s - ";

	bool playing = false;
	void Animate();
	void SetTitle(string title);
	void StopThread();
	void UpdateTitleLength();
};