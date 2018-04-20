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
#include <stdint.h>
#include <thread>
#include <chrono>
#include <mutex>
#include "BufferedAudioReader.h"
#include "MediaTimer.h"

class MusicBot;
using namespace chrono;
using namespace std;

#define RTPB_SAMPLE_TYPE uint16_t
#define RTPB_SAMPLE_RATE 48000
#define RTPB_SAMPLE_FMT AV_SAMPLE_FMT_S16

#define RTPB_CHANNEL_COUNT 2
#define RTPB_CHANNEL_LAYOUT AV_CH_LAYOUT_STEREO

//20ms -> 50Hz
#define RTPB_FRAME_DURATION 20
#define RTPB_FRAME_SIZE (RTPB_FRAME_DURATION * (RTPB_SAMPLE_RATE / 1000)) * sizeof(RTPB_SAMPLE_TYPE) * RTPB_CHANNEL_COUNT

//Number of frames to pre-buffer to maintain smooth playback
#define RTPB_BUFFER_FRAMES 20
#define RTPB_BUFFER_SIZE RTPB_FRAME_SIZE * RTPB_BUFFER_FRAMES + 64000

enum PlaybackState {
	PLAYBACK_STARTING,
	PLAYBACK_RUNNING,
	PLAYBACK_EXITING_ERR,
	PLAYBACK_EXITING_EOS,
	PLAYBACK_EXITING_STOP,
	PLAYBACK_DONE
};

enum PlaybackCommand {
	PLAYBACK_RUN,
	PLAYBACK_STOP,
	PLAYBACK_SEEK,
	PLAYBACK_VOLUME
};

class RealtimePlayback
{
public:
	RealtimePlayback(MusicBot* bot);
	~RealtimePlayback();
	void PlayStream(const string& url,const string &title, bool allowSeek);
	void Seek(uint64_t pos);
	void Stop();
	void Shutdown();
	bool IsPlaying();
	bool CanSeek(uint64_t t); 
	void RegisterVirtualDevice();
	void SetVolume(double volume);
	void ConfigureNormalizer(bool enable, const string &engine, const string &args);

private:
	uint8_t* frameBuffer;
	PlaybackCommand command = PLAYBACK_RUN;
	PlaybackState state = PLAYBACK_DONE;

	void PlayFrame();
	void PlaySamples(RTPB_SAMPLE_TYPE* buffer, size_t sz);

	MusicBot* bot = NULL;
	MediaTimer* timer = NULL;

	thread* playbackThread = NULL;
	BufferedAudioReader* reader = NULL;
	uint64_t seekPosition = 0;
	string playbackTitle;

	void Playback(const string& url, bool allowSeek);

	const char* deviceName = "vDevice";
	const char* deviceId = "vdev10";
	double volume = 1.0;
};