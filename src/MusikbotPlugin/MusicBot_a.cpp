/*#pragma once
#include "MusicBot.h"
#include "plugin_api.h"
#include <time.h>
#include "Logger.h"

MusicBot::MusicBot()
{
	this->buffer = new FiFoBuffer(TRANSPORT_BUFFERSIZE);
	this->reader = new AudioReader(TRANSPORT_SAMPLERATE, TRANSPORT_SAMPLE_FORMAT, TRANSPORT_CHANNEL_LAYOUT);
}

MusicBot::~MusicBot()
{
	delete(this->buffer);
	delete(this->reader);
}

void MusicBot::Initialize()
{

	this->RegisterVirtualDevice();

	/*int error = TS3_OK;
	if ((error = ts3Api.openPlaybackDevice(connection, "custom", this->virtualDeviceId)) != TS3_OK || true) {
		printf("playback device: 0x%x\n", error);
	}
}

void MusicBot::RegisterTs3Functions(TS3Functions ts3Api)
{
	this->ts3Api = ts3Api;
}

bool c = false;
void MusicBot::ProcessChatInput(uint64_t serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored)
{
	if (!this->playing) {
		this->playing = true;

		if (this->playBackThread != NULL) {
			this->playBackThread->join();
			delete(this->playBackThread);
		}

		this->playBackThread = new thread(&MusicBot::Playback, this);

		if (this->monitorThread == NULL) {
			printf("mon start");
			this->monitorThread = new thread(&MusicBot::Monitor, this);
		}

	}
	else {
		this->playing = false;
	}
}

uint32_t hz = 0;
void MusicBot::Monitor()
{
	while (true) {
	Logger::Log(LogLevel_DEBUG, "%u Pakete/s \n", hz);
		hz = 0;
		_sleep(1000);
	}
}

void MusicBot::Playback()
{
	clock_t millis = clock();
	uint32_t transportLen = 0;
	int err = MM_OK;

	this->reader->OpenStream("D:\\samples\\test.mp3");
	Logger::Log(LogLevel_DEBUG, LOG_PLAYBACK_START);

	while (this->playing) {

		uint8_t* transportBuffer = NULL;

		while (this->buffer->GetBufferedBytes() < TRANSPORT_FRAMESIZE && err == MM_OK) {
			err = this->reader->ReadFrame(&transportBuffer, &transportLen);

			if (err != MM_END_OF_STREAM && err != MM_OK) {
				return;
			}
			this->buffer->EnQueue(transportBuffer, transportLen);
		}

		if ((clock() - millis) >= 20) {
			if (this->buffer->GetBufferedBytes() >= TRANSPORT_FRAMESIZE) {
				this->buffer->DeQueue(&transportBuffer, TRANSPORT_FRAMESIZE);
				ts3Api.processCustomCaptureData(this->virtualDeviceId, (short*)transportBuffer, TRANSPORT_SAPMPLES);
				hz++;
				millis = clock();
			}
		}

		if (err == MM_END_OF_STREAM) {
			this->playing = false;
			Logger::Log(LogLevel_DEBUG, LOG_PLAYBACK_EOS);
		}

	}
	Logger::Log(LogLevel_DEBUG, LOG_PLAYBACK_STOP);
	this->reader->CloseStream();
	this->buffer->Clear();
}

void MusicBot::Log(const char * message, LogLevel level, uint64 serverConnectionHandlerID)
{
	ts3Api.logMessage("Playback gestartet.", level, LOG_CHANNEL, serverConnectionHandlerID);
}

void MusicBot::RegisterVirtualDevice()
{
	Logger::Log(LogLevel_DEBUG, LOG_VDEVICE_CREATE, this->virtualDeviceId, this->virtualDeviceName);
	ts3Api.registerCustomDevice(this->virtualDeviceId, this->virtualDeviceName, 48000, 2, 48000, 2);
}
*/