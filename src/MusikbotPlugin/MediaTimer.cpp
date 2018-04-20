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
#include "MediaTimer.h"

MediaTimer::MediaTimer(uint64_t interval, function<void()> tick)
{
	this->tick = tick;
	this->interval = interval;
}

MediaTimer::~MediaTimer()
{
	Stop();
}

void MediaTimer::Start(uint64_t delay)
{
	if (!running) {
		running = true;

#ifdef USE_WINMEDIA
		CreateTimerQueueTimer(&ptrTimerHandle, NULL, TimerCallback, (PVOID)this, (ULONG)delay, (ULONG)interval, WT_EXECUTEDEFAULT);
#else
		waitThread = new thread(&MediaTimer::Run, this);
#endif
	}
}

void MediaTimer::Stop()
{
	if (running) {
		running = false;
#ifdef USE_WINMEDIA
		DeleteTimerQueueTimer(NULL, ptrTimerHandle, NULL);
#else
		waitThread->join();
		delete(waitThread);
		waitThread = NULL;
#endif
	}
}

#ifdef USE_WINMEDIA
void MediaTimer::TimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	if (lpParameter != NULL) {
		((MediaTimer*)lpParameter)->TimerElapsed();
	}
}
#else

void MediaTimer::Run() {
	while (running) {
		TimerElapsed();
	}
}
#endif

void MediaTimer::TimerElapsed() {
	if (tick != NULL) {
		tick();
	}
}