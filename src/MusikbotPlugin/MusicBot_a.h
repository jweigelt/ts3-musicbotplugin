/*#pragma once
#include <ts3_functions.h>
#include <stdint.h>
#include <thread>

using namespace std;

class MusicBot
{
public:
	MusicBot();
	~MusicBot();
	void Initialize();

	//called by Ts3-API
	void RegisterTs3Functions(TS3Functions ts3Api);
	void ProcessChatInput(uint64_t serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored);

private:
	struct TS3Functions ts3Api;
	void Playback();
	void Monitor();
	void Log(const char* message, LogLevel level, uint64 serverConnectionHandlerID);
	void RegisterVirtualDevice();
	char* virtualDeviceName = "vDevice";
	char* virtualDeviceId = "vdev10";
};

*/