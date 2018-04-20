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
#include "AudioException.h"

AudioException::AudioException(const char* message, int ffmpegCode) : runtime_error(message)
{
	//if (ffmpegCode < 0) {
	size_t ll = strlen(message);

	this->message = new char[FFMPEG_ERROR_BUFSZ];
	_strcpy_s(this->message, (rsize_t)FFMPEG_ERROR_BUFSZ, message);

	av_strerror(ffmpegCode, this->message + ll, FFMPEG_ERROR_BUFSZ - ll);
	//}
	//else {
	//	this->message = (char*)malloc(strlen(message) + 1);
	//	strcpy_s(this->message, strlen(message)+1, message);
	//}

	this->ffmpegCode = ffmpegCode;
}

AudioException::AudioException(const char * message) : runtime_error(message)
{
	size_t l = strlen(message) + 2;
	this->message = new char[l];
	_strcpy_s(this->message, l, message);
}

AudioException::~AudioException()
{
	delete[](this->message);
}

int AudioException::Code()
{
	return ffmpegCode;
}