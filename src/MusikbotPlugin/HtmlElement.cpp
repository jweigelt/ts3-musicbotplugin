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
#include "HtmlElement.h"

string HtmlElement::GetAttribute(const string& attributeName)
{
	const string exp = attributeName + "=\"";
	size_t pos1 = html.find(exp);
	if (pos1 == string::npos) return "";
	pos1 += exp.length();
	size_t pos2 = html.find('"', pos1);
	if (pos2 == string::npos) throw runtime_error("Invalid HTML input. End delimiter missing");
	return html.substr(pos1, pos2 - pos1);
}

HtmlElement::HtmlElement(string tagName, string html)
{
	this->tagName = tagName;
	this->html = html;
}

HtmlElement::~HtmlElement()
{
}