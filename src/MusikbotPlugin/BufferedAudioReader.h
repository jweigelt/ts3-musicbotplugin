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
#include "Logger.h"
#include "AudioException.h"
#include "FiFoBuffer.h"

extern "C" {
#include <inttypes.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/avutil.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
}

using namespace std;

typedef void* PlayBuffer();

class BufferedAudioReader
{
public:
	BufferedAudioReader(int outputSampleRate, AVSampleFormat outputFormat, int64_t outputChannelLayout, size_t bufferSz);
	~BufferedAudioReader();

	void OpenStream(const string &mediaSource, bool allowSeek);
	void CloseStream();

	size_t ReadFrame(AVFrame* &out);
	size_t ReadBuffered(uint8_t* out, size_t outLen);
	size_t PreBuffer(size_t minLen);
	size_t BufferedSize();

	void SeekMs(uint64_t t);
	bool CanSeek(uint64_t t);
	uint64_t GetLastFrameTimestamp();

	void SetVolume(double volume);
	void ConfigureNormalizer(bool enable, const string& engine, const string& args);

private:
	AVPacket* packet = NULL;
	AVFrame* frame = NULL;
	FiFoBuffer* buffer = NULL;

	AVFormatContext* formatCtx = NULL;
	AVCodecContext* codecCtx = NULL;

	uint16_t streamIdx;
	AVSampleFormat outputFormat;
	int64_t outputChannelLayout;
	int outputSampleRate;
	int64_t lastFramePosition = 0;

	AVFilterGraph* filterGraph = NULL;
	AVFilterContext* filterInputCtx = NULL;
	AVFilterContext* filterOutputCtx = NULL;

	double volume = 1.0;
	bool normalize = false;
	string normalizeEngine;
	string normalizeArgs;

	bool running = false;
	bool allowSeek = false;
	char* mediaSource = NULL;

	void _OpenStream(const string &mediaSource, bool allowSeek);
	void InitializeFilters(AVCodecContext* ctx);

	void SetupFilter(AVFilterContext* &ctx, const char * filterName, const char* ctxName, const char * format, ...);
	void LinkFilter(AVFilterContext* inCtx, AVFilterContext* ofCtx);

	void DeInitializeFilters();
};