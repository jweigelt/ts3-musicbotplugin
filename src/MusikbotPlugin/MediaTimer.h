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
/*
* Timer-Class for precision event-scheduling
* Uses Windows Media Timers on W32,
* gettimeofday() and usleep() on linux
*/
#pragma once
#include <stdint.h>
#include <functional>
#include <vector>

#ifdef _WIN32
	#define USE_WINMEDIA
#endif

#ifdef USE_WINMEDIA
#include <Windows.h>
#else
#include <thread>
//#include <linux/timer.h>
#endif

using namespace std;
class MediaTimer
{
public:
	MediaTimer(uint64_t interval, function<void()> tick);
	~MediaTimer();
	void Start(uint64_t delay);
	void Stop();

private:
	uint64_t interval = 20;
	function<void()> tick;
	bool running = false;
	void TimerElapsed();

#ifdef USE_WINMEDIA
	HANDLE ptrTimerHandle;
	static void CALLBACK TimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
#else
	thread* waitThread;
	void Run();
#endif

};