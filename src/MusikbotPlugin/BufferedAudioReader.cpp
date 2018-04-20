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
#include "BufferedAudioReader.h"

BufferedAudioReader::BufferedAudioReader(int outputSampleRate, AVSampleFormat outputFormat, int64_t outputChannelLayout, size_t bufferSz)
{
	buffer = new FiFoBuffer(bufferSz);

	this->outputSampleRate = outputSampleRate;
	this->outputFormat = outputFormat;
	this->outputChannelLayout = outputChannelLayout;

	//Register all codecs
	av_register_all();

	//Register all filters
	avfilter_register_all();

	//Initialize network streaming
	avformat_network_init();
}

BufferedAudioReader::~BufferedAudioReader()
{
	CloseStream();
	DeInitializeFilters();
	avformat_network_deinit();
	delete(buffer);
}

void BufferedAudioReader::OpenStream(const string & mediaSource, bool allowSeek)
{
	try
	{
		_OpenStream(mediaSource, allowSeek);
	}
	catch (exception &e) {
		CloseStream();
		throw e;
	}
}

void BufferedAudioReader::_OpenStream(const string& msrc, bool allowSeek)
{
	int err = 0;
	const char* src = msrc.c_str();
	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Playing %s", src);

	if (mediaSource != NULL) {
		delete[](mediaSource);
	}

	mediaSource = new char[strlen(src) + 1];
	_strcpy_s(mediaSource, strlen(src) + 1, src);

	this->allowSeek = allowSeek;

	//Read file header -->
	formatCtx = avformat_alloc_context();

	if ((err = avformat_open_input(&this->formatCtx, this->mediaSource, NULL, NULL)) < 0) {
		throw AudioException("Can't open input stream.", err);
	}

	if ((err = avformat_find_stream_info(this->formatCtx, NULL)) < 0) {
		throw AudioException("Can't find stream header", err);
	}

	//Enable automatic reconnect
	if ((err = av_opt_set(formatCtx, "reconnect", "1", AV_OPT_SEARCH_CHILDREN)) < 0) {
		//throw AudioException("Opt 'reconnect' set failed.", err);
		Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "local file - no reconnect");
	}

	if ((err = av_opt_set(formatCtx, "reconnect_streamed", "1", AV_OPT_SEARCH_CHILDREN)) < 0) {
		//throw AudioException("Opt 'reconnect_streamed' set failed.", err);
	}

	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Input stream ready");
	//<--

	//Find audio stream
	streamIdx = -1;
	for (uint16_t i = 0; i < formatCtx->nb_streams; i++) {
		if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Found audio stream. Index: %u", i);
			streamIdx = i;
			continue;
		}
	}

	if (this->streamIdx < 0) {
		throw AudioException("No audio stream present.");
	}
	//<--

	//Find codec
	AVCodec* codec = avcodec_find_decoder(formatCtx->streams[streamIdx]->codecpar->codec_id);

	if (codec == NULL) {
		throw AudioException("Unknown audio codec.");
	}

	codecCtx = avcodec_alloc_context3(codec);

	if ((err = avcodec_parameters_to_context(codecCtx, formatCtx->streams[streamIdx]->codecpar)) < 0) {
		throw AudioException("Can't set up codecContext", err);
	}

	if ((err = avcodec_open2(codecCtx, codec, NULL)) < 0) {
		throw AudioException("Can't open audio codec.", err);
	}

	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Found audio codec '%s'", codec->name);
	//<--

	packet = av_packet_alloc();
	av_init_packet(packet);

	frame = av_frame_alloc();

	InitializeFilters(codecCtx);


	running = true;
	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Stream initialization complete.");
}

void BufferedAudioReader::CloseStream()
{
	running = false;
	//Free everthing
	if (codecCtx != NULL) {
		avcodec_close(this->codecCtx);
	}

	if (formatCtx != NULL) {
		avformat_close_input(&this->formatCtx);
	}

	if (packet != NULL) {
		av_packet_free(&packet);
	}

	if (frame != NULL) {
		av_frame_free(&frame);
	}

	buffer->Clear();
	DeInitializeFilters();

	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Stream closed.");
}

size_t BufferedAudioReader::ReadFrame(AVFrame* &out)
{
	//unref if this hasn't been done already
	av_frame_unref(frame);

	int err = 0;
	bool got_frame = false;

	while (!got_frame) {

		if ((err = av_read_frame(formatCtx, packet)) < 0) {
			throw AudioException("Can't read frame: ", err);
		}
		else {
			//check if the packet belongs to our audiostream
			if (packet->stream_index == streamIdx) {
				//demuxing and decoding in the same thread
				if ((err = avcodec_send_packet(codecCtx, packet)) < 0) {
					if (err != AVERROR(EAGAIN)) {
						throw AudioException("Decoding error: ", err);
					}
				}

				if ((err = avcodec_receive_frame(codecCtx, frame)) < 0) {
					if (err != AVERROR(EAGAIN)) {
						throw AudioException("Decoding error: ", err);
					}
				}

				else {
					got_frame = true;
				}
			}
			else {
				//free any buffer that doesn't belong to our stream
				av_packet_unref(packet);
			}
		}
	}

	av_packet_unref(packet);

	out = frame;
	return (size_t)av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, codecCtx->sample_fmt, 1);
}

size_t BufferedAudioReader::ReadBuffered(uint8_t * out, size_t outLen)
{
	return buffer->DeQueue(out, outLen);
}

size_t BufferedAudioReader::PreBuffer(size_t minLen)
{
	int err = 0;
	AVFrame* f = NULL;
	bool eos = false;

	while (buffer->Count() < minLen && !eos) {

		if ((err = av_buffersink_get_frame(filterOutputCtx, f)) >= 0) {
			size_t s = (size_t)av_samples_get_buffer_size(NULL, f->channels, f->nb_samples, outputFormat, 1);
			buffer->EnQueue(f->extended_data[0], s);
			av_frame_unref(f);
		}

		if (err == AVERROR(EAGAIN)) {
			size_t frmSz = ReadFrame(f);
			if (frmSz > 0) {
				if ((err = av_buffersrc_add_frame(filterInputCtx, f)) < 0) {		
					av_frame_unref(f);
					throw AudioException("Filter error: ", err);
				}
				av_frame_unref(f);
			}
		}
		else if (err == AVERROR(EOF)) {
			eos = true;
		}
		else if (err < 0) {
			throw AudioException("Filter error.", err);
		}
	}
	return buffer->Count();
}

size_t BufferedAudioReader::BufferedSize()
{
	return buffer->Count();
}

void BufferedAudioReader::SeekMs(uint64_t t) {
	int err = 0;

	if ((err = av_seek_frame(formatCtx, -1, (t / 1000) * AV_TIME_BASE, 0)) < 0) {
		throw AudioException("Seek failed.", err);
	}

	//Empty any buffers to ensure fast seeking
	avcodec_flush_buffers(codecCtx);

	//NOTE: Keeping FiFoBuffer to continue playback while re-buffering (preventing audio stutter)
	//buffer->Clear();

	Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Seeking to %ums", t);
}

bool BufferedAudioReader::CanSeek(uint64_t t)
{
	if (!running) return false;
	uint64_t z = (formatCtx->duration / AV_TIME_BASE) * 1000;
	return (z > t && allowSeek);
}

uint64_t BufferedAudioReader::GetLastFrameTimestamp()
{
	return (lastFramePosition / AV_TIME_BASE) * 1000;
}

void BufferedAudioReader::SetVolume(double volume)
{
	if (volume > 1 || volume < 0) return;

	this->volume = volume;

	if (filterGraph != NULL) {
		char arg[4 + 1];
		_sprintf_s(arg, "%.2f", volume);
		avfilter_graph_send_command(filterGraph, "volume", "volume", arg, NULL, 0, 0);
	}
}

void BufferedAudioReader::ConfigureNormalizer(bool enable, const string & engine, const string & args)
{
	normalize = enable;
	normalizeEngine = engine;
	normalizeArgs = args;
}

void BufferedAudioReader::InitializeFilters(AVCodecContext* ctx)
{
	int err = 0;
	AVFilterContext* aBufferCtx, *volumeCtx, *normalizeCtx, *resampleCtx, *aFormatCtx, *aBufferSinkCtx;

	filterGraph = avfilter_graph_alloc();
	if (filterGraph == NULL) {
		throw AudioException("Out of memory.", AVERROR(ENOMEM));
	}

	SetupFilter(aBufferCtx, "abuffer", "src", "sample_fmt=%s:sample_rate=%d:channel_layout=0x%" PRIx64, av_get_sample_fmt_name(ctx->sample_fmt), ctx->sample_rate, ctx->channel_layout);
	SetupFilter(volumeCtx, "volume", "volume", "volume=%f", volume);
	SetupFilter(resampleCtx, "aresample", "aresample", "%d", outputSampleRate);
	SetupFilter(aFormatCtx, "aformat", "aformat", "sample_fmts=%s:sample_rates=%d:channel_layouts=0x%" PRIx64, av_get_sample_fmt_name(outputFormat), outputSampleRate, outputChannelLayout);
	SetupFilter(aBufferSinkCtx, "abuffersink", "sink", NULL);

	if (normalize) {
		Logger.Log(LogLevel_DEBUG, LOGSENDER_AUDIO, "Setting up '%s' for audio normalization.", normalizeEngine.c_str());
		SetupFilter(normalizeCtx, normalizeEngine.c_str() , normalizeEngine.c_str(), normalizeArgs.c_str());
		LinkFilter(aBufferCtx, normalizeCtx);
		LinkFilter(normalizeCtx, volumeCtx);
	}
	else {
		LinkFilter(aBufferCtx, volumeCtx);
	}

	LinkFilter(volumeCtx, resampleCtx);
	LinkFilter(resampleCtx, aFormatCtx);
	LinkFilter(aFormatCtx, aBufferSinkCtx);

	if ((err = avfilter_graph_config(filterGraph, NULL)) < 0) {
		throw AudioException("Failed to configure filter graph.", err);
	}

	filterInputCtx = aBufferCtx;
	filterOutputCtx = aBufferSinkCtx;
}

void BufferedAudioReader::SetupFilter(AVFilterContext *& ctx, const char * filterName, const char * ctxName, const char * format, ...)
{
	AVFilter *filter = NULL;
	int err = 0;
	va_list args;
	char options[1024];

	filter = avfilter_get_by_name(filterName);
	if (filter == NULL) {
		throw AudioException("Can't find abuffer filter.", AVERROR_FILTER_NOT_FOUND);
	}

	ctx = avfilter_graph_alloc_filter(filterGraph, filter, ctxName);
	if (ctx == NULL) {
		throw AudioException("Out of memory.", AVERROR(ENOMEM));
	}

	if (format != NULL) {
		va_start(args, format);
		vsnprintf(options, sizeof(options), format, args);
		va_end(args);
	}

	if ((err = avfilter_init_str(ctx, (format == NULL ? NULL : options))) < 0) {
		throw AudioException("Couldn't initialize filter.", err);
	}
}

void BufferedAudioReader::LinkFilter(AVFilterContext * inCtx, AVFilterContext * ofCtx)
{
	int err = 0;
	if ((err = avfilter_link(inCtx, 0, ofCtx, 0)) < 0) {
		throw AudioException("Failed to link filter chain.", err);
	}
}

void BufferedAudioReader::DeInitializeFilters()
{
	if (filterGraph != NULL) {
		avfilter_graph_free(&filterGraph);
	}
}