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
//#define OUTPUT_SAMPLE_FMT AV_SAMPLE_FMT_S16
//#define OUTPUT_CHANNEL_LAYOUT AV_CH_LAYOUT_STEREO
#pragma once

#define MM_OK					0
#define MM_CANT_OPEN_STREAM		1
#define MM_UNKOWN_FORMAT		2
#define MM_NO_AUDIO_STREAM		3
#define MM_UNKOWN_CODEC			4
#define MM_CANT_OPEN_CODEC		5
#define MM_CANT_INIT_RESAMPLER  6
#define MM_END_OF_STREAM		7
#define MM_ALLOC_FAILED			8 
#define MM_ENCODE_FAILED		10

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
}
#endif