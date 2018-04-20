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
#include "RealtimePlayback.h"
#include "MusicBot.h"

RealtimePlayback::RealtimePlayback(MusicBot * bot)
{
	this->bot = bot;
	timer = new MediaTimer(RTPB_FRAME_DURATION, bind(&RealtimePlayback::PlayFrame, this));
	reader = new BufferedAudioReader(RTPB_SAMPLE_RATE, RTPB_SAMPLE_FMT, AV_CH_LAYOUT_STEREO, RTPB_BUFFER_SIZE);
	frameBuffer = new uint8_t[RTPB_FRAME_SIZE];
}

RealtimePlayback::~RealtimePlayback()
{
	delete(reader);
	delete(timer);
	delete[](frameBuffer);
}

void RealtimePlayback::PlayStream(const string & url, const string & title, bool allowSeek)
{
	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Starting playback...");
	if (state != PLAYBACK_DONE || playbackThread != NULL) {
		Stop();
	}

	command = PLAYBACK_RUN;
	state = PLAYBACK_STARTING;
	playbackTitle = title;
	playbackThread = new thread(&RealtimePlayback::Playback, this, url, allowSeek);
}

void RealtimePlayback::Seek(uint64_t pos)
{
	seekPosition = pos;
	command = PLAYBACK_SEEK;
}

void RealtimePlayback::Stop()
{
	command = PLAYBACK_STOP;

	if (playbackThread != NULL) {
		playbackThread->join();
		delete(playbackThread);
		playbackThread = NULL;
	}
}

void RealtimePlayback::Shutdown()
{
	Stop();
}

bool RealtimePlayback::IsPlaying()
{
	return (state != PLAYBACK_DONE);
}

bool RealtimePlayback::CanSeek(uint64_t t)
{
	if (state == PLAYBACK_RUNNING) {
		return reader->CanSeek(t);
	}
	else {
		return false;
	}
}

void RealtimePlayback::RegisterVirtualDevice()
{
	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Registering virtual device '%s' with id '%s'", deviceName, deviceId);
	bot->GetApi()->registerCustomDevice(deviceId, deviceName, RTPB_SAMPLE_RATE, RTPB_CHANNEL_COUNT, RTPB_SAMPLE_RATE, RTPB_CHANNEL_COUNT);
}

void RealtimePlayback::SetVolume(double volume)
{
	this->volume = volume;
	command = PLAYBACK_VOLUME;
}

void RealtimePlayback::ConfigureNormalizer(bool enable, const string & engine, const string & args)
{
	reader->ConfigureNormalizer(enable, engine, args);
}

void RealtimePlayback::PlayFrame()
{
	size_t l = reader->ReadBuffered(frameBuffer, RTPB_FRAME_SIZE);
	if (l > 0) {
		PlaySamples((RTPB_SAMPLE_TYPE*)frameBuffer, l);
	}
}

void RealtimePlayback::Playback(const string & url, bool allowSeek)
{
	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Playbackthread: Hello");
	memset((void*)frameBuffer, 0, RTPB_FRAME_SIZE);

	state = PLAYBACK_RUNNING;

	try {
		reader->OpenStream(url, allowSeek);
	}
	catch (AudioException &e) {
		Logger.Log(LogLevel_ERROR, LOGSENDER_AUDIO, "Failed to open stream (%s). Exiting playback.", e.what());
		state = PLAYBACK_EXITING_ERR;
	}

	if (state == PLAYBACK_RUNNING) {
		timer->Start(RTPB_FRAME_DURATION);
	}

	bot->GetAnimationProvider()->UpdatePlaybackTitle(playbackTitle);

	while (state == PLAYBACK_RUNNING) {
		if (command == PLAYBACK_STOP) {
			state = PLAYBACK_EXITING_STOP;
		}

		if (command == PLAYBACK_SEEK) {
			command = PLAYBACK_RUN;

			try {
				reader->SeekMs(seekPosition);
			}
			catch (AudioException &e) {
				Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Seek failed: %s", e.what());
			}
		}

		if (command == PLAYBACK_VOLUME) {
			command = PLAYBACK_RUN;
			reader->SetVolume(volume);
		}


		try {
			reader->PreBuffer(RTPB_BUFFER_FRAMES * RTPB_FRAME_SIZE);
		}
		catch (AudioException &e) {
			if (e.Code() == AVERROR_EOF) {
				Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Reached end of stream. Stop.");
				state = PLAYBACK_EXITING_EOS;
			}
			else {
				Logger.Log(LogLevel_WARNING, LOGSENDER_AUDIO, e.what());
				state = PLAYBACK_EXITING_ERR;
			}
		}
		this_thread::sleep_for(chrono::milliseconds(RTPB_FRAME_DURATION));
	}
	while ((state == PLAYBACK_EXITING_EOS && reader->BufferedSize() > 0)) this_thread::sleep_for(50ms);

	timer->Stop();
	reader->CloseStream();
	state = PLAYBACK_DONE;
	bot->GetAnimationProvider()->ResetTitle();
	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Playbackthread: Exiting");
}

void RealtimePlayback::PlaySamples(RTPB_SAMPLE_TYPE* buffer, size_t sz)
{
	bot->GetApi()->processCustomCaptureData(deviceId, (short*)buffer, sz / 4);
}