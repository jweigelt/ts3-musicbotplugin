/*
* This file is part of Music Bot Plugin. (https://github.com/jweigelt/ts3-musicbotplugin)
* Copyright (C) 2017 Jan Weigelt (jan@janelo.net)
*
* usic Bot Plugin is free software: you can redistribute it and/or modify
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

#include "stdafx.h"
#include "plugin_api.h"

#include <teamspeak\public_rare_definitions.h>
#include <iostream>
#include <string>
#include <ao\ao.h>

using namespace std;

#define TS3_OK 0
#define SCHID 1
#define CLID 1
#define FROMID 2
#define FROMNAME "Test"
#define FROMUQID "Test-01"
#define CLIENT_LIST_GROUPS "1,2"

ao_device* device = NULL;

char* buf = NULL;


unsigned int registerCustomDevice(const char* deviceID, const char* deviceDisplayName, int capFrequency, int capChannels, int playFrequency, int playChannels) {
	cout << "Device registration called for " << deviceID << "\n";
	return  TS3_OK;
}

unsigned int getClientID(uint64 serverConnectionHandlerID, anyID* result) {
	*result = CLID;
	return TS3_OK;
}

unsigned int getClientVariableAsString(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, char** result) {
	if (flag == CLIENT_SERVERGROUPS) {
		*result = CLIENT_LIST_GROUPS;
	}
	return TS3_OK;
}

unsigned int flushClientSelfUpdates(uint64 serverConnectionHandlerID, const char* returnCode) {
	return TS3_OK;
}

unsigned int setClientSelfVariableAsString(uint64 serverConnectionHandlerID, size_t flag, const char* value) {
	return TS3_OK;
}

unsigned int processCustomCaptureData(const char* deviceName, const short* buffer, int samples) {
	size_t sz = samples * sizeof(short) * 2;
	char* buf = new char[sz];
	memcpy(buf, buffer, sz);
	ao_play(device, buf, sz);
	delete(buf);
	return TS3_OK;
}

unsigned int requestSendPrivateTextMsg(uint64 serverConnectionHandlerID, const char* message, anyID targetClientID, const char* returnCode) {
	cout << "Received private Message: " << message << "\n";
	return TS3_OK;
}

int main()
{
	cout << "Preparing Playback device" << "\n";

	ao_sample_format format;

	ao_initialize();
	memset(&format, 0, sizeof(format));
	format.bits = 16;
	format.channels = 2;
	format.rate = 48000;
	format.byte_format = AO_FMT_NATIVE;

	int default_driver = ao_default_driver_id();
	device = ao_open_live(default_driver, &format, NULL);

	cout << "Loading Plugin " << ts3plugin_name() << " v." << ts3plugin_version() << "\n";
	TS3Functions ff = TS3Functions();
	ff.registerCustomDevice = registerCustomDevice;
	ff.getClientID = getClientID;
	ff.getClientVariableAsString = getClientVariableAsString;
	ff.flushClientSelfUpdates = flushClientSelfUpdates;
	ff.setClientSelfVariableAsString = setClientSelfVariableAsString;
	ff.processCustomCaptureData = processCustomCaptureData;
	ff.requestSendPrivateTextMsg = requestSendPrivateTextMsg;
	ts3plugin_setFunctionPointers(ff);
	ts3plugin_init();

	cout << "Emulating connect..." << "\n";
	for (int i = 0; i < 3; i++) {
		char groupName[32];
		sprintf(groupName, "ServerGroup-%i", i);
		ts3plugin_onServerGroupListEvent(SCHID, i, groupName, 0, 0, 0);
	}
	ts3plugin_onServerGroupListFinishedEvent(SCHID);

	cout << "OK." << "\n";
	string s;
	do {
		getline(cin, s);
		if (s != "quit") {
			ts3plugin_onTextMessageEvent(SCHID, TextMessageTarget_CLIENT, CLID, FROMID, FROMNAME, FROMUQID, s.c_str(), 0);
		}
	} while (s != "quit");

	cout << "Shutting down..." << "\n";
	ts3plugin_shutdown();
	ao_close(device);
	ao_shutdown();
	return 0;
}