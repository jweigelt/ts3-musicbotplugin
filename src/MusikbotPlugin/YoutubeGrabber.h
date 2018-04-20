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

#include <string>
#include <regex>
#include <functional>
#include <thread>
#include <vector>
#include <stdexcept>

#include <curl/curl.h>
#include "HtmlElement.h"
#include "Ts3User.h"
#include "Logger.h"

using namespace std;

#define JS_ENCODER_EXP "=function(a){a=a.split("

size_t __curl_write_callback(void * contents, size_t size, size_t nmemb, void * userp);

struct HttpResponse {
	char* buffer;
	size_t bufferSz;

	HttpResponse() {
		buffer = NULL;
		bufferSz = 0;
	}
	~HttpResponse() {
		delete[] buffer;
	}
};

struct YoutubeVideoInfo
{
	string title;
	string url;
	string quality;
	string type;
	string fallback_host;
	string signature;
};

typedef void(*DecodeFunction)(string &sig, int idx);

struct DecodeFunctionCall {
	DecodeFunction func;
	string name;
	int idx;
};

class YoutubeGrabber
{
public:
	YoutubeGrabber();
	~YoutubeGrabber();

	vector<YoutubeVideoInfo> GetVideoList(const string& url);

	void GetVideoInfoAsync(const string& url, const Ts3User &user);
	bool IsRunning();
	void SetGetOkCallback(function<void(vector<YoutubeVideoInfo>&, const Ts3User&)> cb);
	void SetGetFailedCallback(function<void(const exception&, const Ts3User&)> cb);
	vector <string> GetPlaylist(HttpResponse &site);

private:
	void CurlHttpGet(const string& url, HttpResponse &response);

	vector<HtmlElement> GetElementsByAttrMatch(const string& attr, const string& val, HttpResponse& site, int max = -1);
	HtmlElement GetElementById(const string& val, HttpResponse& site);
	string GetJsonVal(const string& key, HttpResponse& site);
	string UrlDecode(const string& html);
	string GetUrlParam(const string & url, const string& paramName);
	string GetTitle(HttpResponse& site);
	vector<DecodeFunctionCall> GetDecodeAlg(HttpResponse& site);
	DecodeFunctionCall FindFunction(const string& row, vector<DecodeFunctionCall> cache, const string& player);

	void GetVideoInfo(const string & url, const Ts3User & user);
	function<void(vector<YoutubeVideoInfo>&, const Ts3User&)> onGetOk = NULL;
	function<void(const exception&, const Ts3User&)> onGetFailed = NULL;
	thread* workThread = NULL;
	bool running = false;
};