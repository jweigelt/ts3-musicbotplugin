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
#include <stdexcept>
#include <SimpleIni.h>

using namespace std;

class ConfigException : public runtime_error
{
public:
	ConfigException(const char* message, SI_Error siCode);
	ConfigException(const char* message);
	~ConfigException();
	virtual const char* what() const throw() {
		return message;
	}
private:
	SI_Error siCode = SI_OK;
	const char* message = NULL;
};