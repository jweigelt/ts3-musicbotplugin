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
#include "NameAnimationProvider.h"
#include "MusicBot.h"

NameAnimationProvider::NameAnimationProvider(MusicBot * bot)
{
	this->bot = bot;
	UpdateTitleLength();
}

NameAnimationProvider::~NameAnimationProvider()
{
	StopThread();
}

void NameAnimationProvider::UpdatePlaybackTitle(string const & s)
{
	if (animationThread != NULL) {
		ResetTitle();
	}

	switch (animation)
	{
	case ANIMATION_FLOW:
		playbackTitle = Utility::FormatString(flowFormat, s.c_str());
		break;

	case ANIMATION_SWITCH:
		playbackTitle = Utility::FormatString(switchFormat, s.c_str());
		break;

	case ANIMATION_CUTOFF:
		playbackTitle = s;
		SetTitle(playbackTitle);
		return;
		break;

	case ANIMATION_NONE:
	default:
		return;
		break;
	}

	playing = true;
	animationThread = new thread(&NameAnimationProvider::Animate, this);
}

void NameAnimationProvider::ResetTitle()
{
	StopThread();
	animationIndex = 0;
	playbackTitle = string();
	SetTitle(playbackTitle);
}

void NameAnimationProvider::SetNickName(const string & nickName)
{
	this->nickName = nickName;
	UpdateTitleLength();
}

void NameAnimationProvider::SetNameFormat(const string & nameFormat)
{
	this->nameFormat = nameFormat;
	UpdateTitleLength();
}

void NameAnimationProvider::SetSwitchFormat(const string & switchFormat)
{
	this->switchFormat = switchFormat;
}

void NameAnimationProvider::SetFlowFormat(const string & flowFormat)
{
	this->flowFormat = flowFormat;
}

void NameAnimationProvider::SetAnimation(Animation animation)
{
	this->animation = animation;
}

void NameAnimationProvider::SetFlowDelay(uint32_t flowDelay)
{
	this->flowDelay = flowDelay;
}

void NameAnimationProvider::SetSwitchDelay(uint32_t switchDelay)
{
	this->switchDelay = switchDelay;
}

void NameAnimationProvider::Shutdown()
{
	StopThread();
}

void NameAnimationProvider::Animate()
{
	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Animationthread: Hello");
	uint32_t animationDelay = (animation == ANIMATION_FLOW ? flowDelay : switchDelay);

	while (playing) {
		string t = string();

		switch (animation) {
		case ANIMATION_SWITCH:
			if (playbackTitle.length() <= (animationIndex + titleLength)) {
				if (animationIndex < playbackTitle.length()) {
					t = playbackTitle.substr(animationIndex);
					animationIndex = 0;
				}
				else {
					animationIndex = 0;
					t = playbackTitle.substr(animationIndex, titleLength);
				}
			}
			else {
				t = playbackTitle.substr(animationIndex, titleLength);
				animationIndex += titleLength;
			}
			break;

		case ANIMATION_FLOW:
			if (animationIndex >= playbackTitle.length()) animationIndex = 0;

			if (playbackTitle.length() > animationIndex + titleLength) {
				t = playbackTitle.substr(animationIndex, titleLength);
			}
			else {
				t = playbackTitle.substr(animationIndex);
				size_t additionalLen = (animationIndex + titleLength) - playbackTitle.length();
				t += playbackTitle.substr(0, additionalLen);
			}
			animationIndex++;
			break;
		}

		SetTitle(t);

		unique_lock<mutex> ul(mtx);
		cv.wait_for(ul, milliseconds(animationDelay), [this] { return !playing; });
		ul.unlock();
	}

	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Animationthread: Exiting");
}

void NameAnimationProvider::SetTitle(string title)
{
	string t = string();

	if (!title.empty()) {
		t = Utility::FormatString(nameFormat, nickName.c_str(), title.c_str());
		if (t.length() > TS_MAX_NAME_LEN) t = t.substr(0, TS_MAX_NAME_LEN);
	}
	else {
		t = nickName;
	}

	bot->GetApi()->setClientSelfVariableAsString(bot->GetServerConnectionHandlerID(), CLIENT_NICKNAME, t.c_str());
	bot->GetApi()->flushClientSelfUpdates((uint64)bot->GetServerConnectionHandlerID(), NULL);
}

void NameAnimationProvider::StopThread()
{
	if (animationThread == NULL) return;
	if (!animationThread->joinable()) return;

	{
		lock_guard<mutex> lg(mtx);
		playing = false;
		cv.notify_one();
	}

	animationThread->join();
	delete(animationThread);
	animationThread = NULL;
}

void NameAnimationProvider::UpdateTitleLength()
{
	string s = Utility::FormatString(nameFormat, nickName.c_str(), "");
	if (s.size() > TS_MAX_NAME_LEN) {
		Logger.Log(LogLevel_ERROR, LOGSENDER_CONFIG, "Name format string length exceeds Ts3's max. name length.");
		titleLength = 0;
	}
	else {
		titleLength = (uint8_t)(TS_MAX_NAME_LEN - s.size());
	}
}