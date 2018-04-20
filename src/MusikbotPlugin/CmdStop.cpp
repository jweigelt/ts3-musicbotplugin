
#include "CmdStop.h"
#include "MusicBot.h"


CmdStop::CmdStop() :RemoteCommand("stop", "stop", "stop", "stop")
{
}

CmdStop::~CmdStop()
{
}

bool CmdStop::Execute(vector<string> const & message, Ts3User const & user)
{
	if (message.size() != 1) return false;
	
	if (GetBot()->GetPlayback()->IsPlaying()) {
		GetBot()->GetPlayback()->Stop();
		SendToUser(user, onStop);
	}
	else {
		SendToUser(user, onNotPlaying);
	}
	return true;
}

void CmdStop::SetDefaultConfig(ConfigReader * reader)
{
	reader->SetString(INI_SECTION_TEMPLATE, "onstop", NULL, onStop);
	reader->SetString(INI_SECTION_TEMPLATE, "onnotplaying", NULL, onNotPlaying);
}

void CmdStop::ReadConfig(ConfigReader * reader)
{
	onStop = reader->GetString(INI_SECTION_TEMPLATE, "onstop");
	onNotPlaying = reader->GetString(INI_SECTION_TEMPLATE, "onnotplaying");
}