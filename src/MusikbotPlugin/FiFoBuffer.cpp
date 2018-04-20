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
//check threadsafe
#include "FiFoBuffer.h"

FiFoBuffer::FiFoBuffer(size_t bufferLength)
{
	this->bufferLength = bufferLength;
	buffer = new uint8_t[bufferLength];
	bufferedBytes = 0;
	readIndex = 0;
	writeIndex = 0;
}

FiFoBuffer::~FiFoBuffer()
{
	delete[](buffer);
}

void FiFoBuffer::EnQueue(uint8_t* src, size_t len)
{
	if (len > (bufferLength - bufferedBytes)) throw AudioException("Buffer overflow.");

	lock_guard<mutex> lg(mtx);
	{
		if (writeIndex + len > bufferLength) {
			memcpy(buffer, buffer + readIndex, writeIndex - readIndex);
			writeIndex = writeIndex - readIndex;
			readIndex = 0;
		}

		memcpy(buffer + writeIndex, src, len);
		writeIndex += len;
		bufferedBytes += len;
	}
}

void FiFoBuffer::Clear()
{
	lock_guard<mutex> lg(mtx);
	{
		bufferedBytes = 0;
		writeIndex = 0;
		readIndex = 0;
	}
}

size_t FiFoBuffer::DeQueue(uint8_t* dst, size_t len)
{
	if (bufferedBytes < len) {
		len = bufferedBytes;
	}

	lock_guard<mutex> lg(mtx);
	{
		memcpy(dst, buffer + readIndex, len);
		readIndex += len;
		bufferedBytes -= len;
	}
	return len;
}

size_t FiFoBuffer::Count()
{
	return this->bufferedBytes;;
}