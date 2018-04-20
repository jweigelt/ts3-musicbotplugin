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
#include <string.h>
#include <cstdlib>
#include <mutex>
#include "AudioException.h"

class FiFoBuffer
{
public:
	FiFoBuffer(size_t bufSz);
	~FiFoBuffer();
	void EnQueue(uint8_t* src, size_t len);
	size_t DeQueue(uint8_t* dst, size_t len);
	void Clear();
	size_t Count();
private:
	uint8_t* buffer;
	size_t bufferLength;
	size_t bufferedBytes;
	size_t readIndex;
	size_t writeIndex;
	mutex mtx;
};