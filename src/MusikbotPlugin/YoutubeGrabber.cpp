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
#include "YoutubeGrabber.h"
#include "DebugHelper.h"

#define HTML_SFIND_S(_str,_var, _exp, _off, _err) _var=_str.find(_exp, _off); if(_var == string::npos){throw runtime_error(_err); }
#define HTML_SRFIND_S(_str,_var, _exp, _off, _err) _var=_str.rfind(_exp, _off); if(_var == string::npos){throw runtime_error(_err); }
#define CHECK_MALLOC(x)

//TODO: Rewrite using C++11 functions
void __encode_reverse(string &sig, int idx) {
	string temp = string(sig.rbegin(), sig.rend());
	sig = temp;
}
void __encode_splice(string &sig, int idx) {
	string temp = sig.substr(idx, sig.length() - idx);
	sig = temp;
}
void __encode_swap(string &sig, int idx) {
	char c = sig[0];
	sig[0] = sig[idx % sig.length()];
	sig[idx] = c;
}

size_t __curl_write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t bytes = size*nmemb;
	HttpResponse* buf = (HttpResponse*)userp;

	buf->buffer = (char*)realloc(buf->buffer, buf->bufferSz + bytes + 1);
	if (buf->buffer == NULL) { //OOM
		return 0;
	}

	memcpy((void*)(buf->buffer + buf->bufferSz), contents, bytes);

	buf->bufferSz += bytes;
	buf->buffer[buf->bufferSz] = 0; //string termination

	return bytes;
}

void YoutubeGrabber::CurlHttpGet(const string & url, HttpResponse & response)
{
	CURL *curl;
	CURLcode res;
	response = HttpResponse();

	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __curl_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			throw runtime_error(curl_easy_strerror(res));
		}
	}
	else
	{
		throw runtime_error("CURL couldn't be initialized.");
	}
}

vector<HtmlElement> YoutubeGrabber::GetElementsByAttrMatch(const string& attr, const string& val, HttpResponse& site, int max)
{
	vector<HtmlElement> res = vector<HtmlElement>();
	const string s = site.buffer;
	size_t np = 0;

	size_t tagStart = 0;
	size_t tagNameEnd = 0;
	size_t tagEnd = 0;
	string tagName;
	string endTag;
	string element;

	while ((np = s.find(attr + "=\"" + val + "\"", tagEnd)) != string::npos) {
		HTML_SRFIND_S(s, tagStart, '<', np, "tagStart not found.");
		HTML_SFIND_S(s, tagNameEnd, ' ', tagStart, "tagNameEnd not found.");
		tagName = s.substr(tagStart + 1, tagNameEnd - tagStart - 1);
		endTag = "</" + tagName + ">";
		HTML_SFIND_S(s, tagEnd, endTag, tagStart, "tagEnd not found.");
		element = s.substr(tagStart, tagEnd - tagStart + endTag.length());
		res.push_back(HtmlElement(tagName, element));
	}
	return res;
}

HtmlElement YoutubeGrabber::GetElementById(const string & val, HttpResponse & site)
{
	vector<HtmlElement> res = GetElementsByAttrMatch("id", val, site, 1);
	if (res.size() < 1) throw runtime_error("element not found");
	return res[0];
}

string YoutubeGrabber::GetJsonVal(const string& key, HttpResponse& site) {
	size_t start = 0;
	size_t stop = 0;
	const string s = site.buffer;
	const string exp = "\"" + key + "\":\"";
	HTML_SFIND_S(s, start, exp, 0, "Json start not found.");
	start += exp.length();
	HTML_SFIND_S(s, stop, "\"", start, "Json end of input not found");
	string res = s.substr(start, stop - start);
	return res;
}

string YoutubeGrabber::UrlDecode(const string &html)
{
	CURL *curl;
	curl = curl_easy_init();
	if (curl) {
		int outlen = 0;
		const char* r = curl_easy_unescape(curl, html.c_str(), html.length(), &outlen);
		curl_easy_cleanup(curl);
		return (string)r;
	}
	else
	{
		throw runtime_error("CURL couldn't be initialized.");
	}
}

string YoutubeGrabber::GetUrlParam(const string & paramName, const string & url)
{
	size_t pos1 = 0;
	const string exp = paramName + "=";
	pos1 = url.find(exp, 0);
	if (pos1 == string::npos) return "";
	pos1 += exp.length();
	size_t pos2 = url.find('&', pos1);
	return (pos2 == string::npos ? url.substr(pos1) : url.substr(pos1, pos2 - pos1));
}

string YoutubeGrabber::GetTitle(HttpResponse& site)
{
	const string s = site.buffer;
	size_t np = 0;
	size_t tagStart = 0;
	size_t tagEnd = 0;

	tagStart = s.find("<title>") + strlen("<title>");
	if (tagStart == string::npos) return string();
	tagEnd = s.find("</title>", tagStart);
	if (tagEnd == string::npos) return string();

	return s.substr(tagStart, tagEnd - tagStart);
}

vector<DecodeFunctionCall> YoutubeGrabber::GetDecodeAlg(HttpResponse& site)
{
	vector<HtmlElement> players = GetElementsByAttrMatch("name", "player/base", site);
	if (players.size() == 0) throw runtime_error("Couldn't find YT Playerbase.");

	string playerUrl = "https://youtube.com/" + players.at(0).GetAttribute("src");
	HttpResponse jsPlayer = HttpResponse();
	CurlHttpGet(playerUrl, jsPlayer);

	const string p = jsPlayer.buffer;
	size_t encoderStart = 0;
	size_t encoderEnd = 0;
	HTML_SFIND_S(p, encoderStart, "=function(a){a=a.split(\"\");", 0, "Player-decode function not found.");
	HTML_SFIND_S(p, encoderStart, "{", encoderStart, "Player-decode opening { not found");
	HTML_SFIND_S(p, encoderEnd, "}", encoderStart, "closing } not found");

	string decoder = p.substr(encoderStart, encoderEnd - encoderStart);

	vector<DecodeFunctionCall> res = vector<DecodeFunctionCall>();
	size_t lineStart = 0;
	size_t lineEnd = 0;
	string row;

	do {
		lineEnd = decoder.find(';', lineStart);
		row = (lineEnd == string::npos ? decoder.substr(lineStart) : decoder.substr(lineStart, lineEnd - lineStart));
		lineStart = lineEnd + 1;
		if (row.find("split") == string::npos && row.find("join") == string::npos) {
			res.push_back(FindFunction(row, res, p));
		}

	} while (lineEnd != string::npos);

	return res;
}

vector<YoutubeVideoInfo> YoutubeGrabber::GetVideoList(const string& url) {
	HttpResponse webPage = HttpResponse();
	CurlHttpGet(url, webPage);
	bool decoderRequired = false;
	//BinaryDump((uint8_t*)webPage.buffer);
	//HttpResponse webPage = HttpResponse();
	//webPage.buffer = (char*)BinaryRead();

	string s = GetJsonVal("url_encoded_fmt_stream_map", webPage);
	s = regex_replace(s, regex("\\\\u0026"), "&");;

	size_t pos1 = 0;
	size_t pos2 = 0;
	string row;

	string title = GetTitle(webPage);

	vector<YoutubeVideoInfo> videos = vector<YoutubeVideoInfo>();
	do {
		pos2 = s.find(',', pos1);
		row = (pos2 == string::npos ? s.substr(pos1) : s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;

		YoutubeVideoInfo info = YoutubeVideoInfo();
		info.url = GetUrlParam("url", row);
		info.signature = GetUrlParam("s", row);
		info.fallback_host = GetUrlParam("fallback_host", row);
		info.type = UrlDecode(GetUrlParam("type", row));
		info.quality = GetUrlParam("quality", row);
		info.title = title;

		if (info.signature.empty()) info.signature = GetUrlParam("sig", row);

		if (!info.signature.empty())
		{
			decoderRequired = true;
			//info.url += "&signature=" + info.signature;
			if (!info.fallback_host.empty()) info.url += "&fallback_host=" + info.fallback_host;
		}

		info.url = UrlDecode(info.url);
		info.url = UrlDecode(info.url);

		videos.push_back(info);

	} while (pos2 != string::npos);

	if (decoderRequired) {
		vector<DecodeFunctionCall> decodeAlg = GetDecodeAlg(webPage);

		for (YoutubeVideoInfo &v : videos) {
			for (DecodeFunctionCall &c : decodeAlg) {
				c.func(v.signature, c.idx);

			}
			v.url += "&signature=" + v.signature;
		}
	}

	return videos;
}

DecodeFunctionCall YoutubeGrabber::FindFunction(const string& row, vector<DecodeFunctionCall> cache, const string& player) {
	DecodeFunctionCall call = DecodeFunctionCall();
	size_t nameEnd = 0;
	HTML_SFIND_S(row, nameEnd, "(", 0, "Can't parse decode function");
	string name = row.substr(0, nameEnd);
	size_t pt = name.find('.');
	if (pt != string::npos)name = name.substr(pt + 1);

	size_t pc = 0;
	HTML_SFIND_S(row, pc, ',', pt, "Can't find idx in function call");
	pc++; //Skip ','
	size_t cb = 0;
	HTML_SFIND_S(row, cb, ')', pc, "Can't find idx in function call");
	string idx = row.substr(pc, cb - pc);
	call.idx = stoi(idx);


	for (DecodeFunctionCall &c : cache) {
		if (c.name.compare(name) == 0) {
			call.func = c.func;
			call.name = name;
			return call;
		}
	}

	size_t funcStart = 0;
	size_t funcEnd = 0;
	const string exp = name + ":function";
	HTML_SFIND_S(player, funcStart, exp, 0, "Can't find decode function.");
	funcStart += exp.length();
	HTML_SFIND_S(player, funcEnd, "}", funcStart, "Can't find decode function closing }.");

	string function = player.substr(funcStart, funcEnd - funcStart);

	call.name = name;

	if (function.find("splice") != string::npos) {
		call.func = __encode_splice;
	}
	else if (function.find("reverse") != string::npos) {
		call.func = __encode_reverse;
	}
	else {
		call.func = __encode_swap;
	}

	return call;
}

YoutubeGrabber::YoutubeGrabber() { }

YoutubeGrabber::~YoutubeGrabber()
{
	if (workThread != NULL) {
		workThread->join();
		delete(workThread);
	}
}

void YoutubeGrabber::GetVideoInfoAsync(const string & url, const Ts3User & user)
{
	if (running) throw runtime_error("already running");

	if (workThread != NULL) {
		if (workThread->joinable()) {
			workThread->join();
		}
		delete(workThread);
		workThread = NULL;
	}

	workThread = new thread(&YoutubeGrabber::GetVideoInfo, this, url, user);
}

void YoutubeGrabber::GetVideoInfo(const string & url, const Ts3User & user)
{
	Logger.Log(LogLevel_DEBUG, LOGSENDER_YOUTUBE, "Youtubethread: Hello");
	try {
		vector<YoutubeVideoInfo> videos = GetVideoList(url);
		onGetOk(videos, user);
	}
	catch (exception& e) {
		Logger.Log(LogLevel_WARNING, LOGSENDER_YOUTUBE, "Youtube videolist retrieval failed: %s", e.what());
		if (onGetFailed != NULL) onGetFailed(e, user);
	}
	Logger.Log(LogLevel_DEBUG, LOGSENDER_YOUTUBE, "Youtubethread: Exiting");
}

bool YoutubeGrabber::IsRunning()
{
	return running;;
}

void YoutubeGrabber::SetGetOkCallback(function<void(vector<YoutubeVideoInfo>&, const Ts3User&)> cb)
{
	onGetOk = cb;
}

void YoutubeGrabber::SetGetFailedCallback(function<void(const exception&, const Ts3User&)> cb)
{
	onGetFailed = cb;
}

vector<string> YoutubeGrabber::GetPlaylist(HttpResponse & site)
{
	HtmlElement e = GetElementById("playlist-autoscroll-list", site);
	return vector<string>();
}