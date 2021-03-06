#include SAMPEDGENGINE_PCH

#include <SAMP-EDGEngine/Server/ServerDebugLogOutput.hpp>

#include <SAMP-EDGEngine/Server/GameMode.hpp>

#include <SAMP-EDGEngine/Core/TextInc.hpp>

namespace samp_edgengine
{

#ifdef DEBUG

///////////////////////////////////////////////////////////
ServerDebugLogOutput::ServerDebugLogOutput(IGameMode & gameMode_)
	: IGameModeChild(gameMode_)
{
}

///////////////////////////////////////////////////////////
void ServerDebugLogOutput::push(LogMessage messageType_, std::string message_)
{
	namespace ascii = text::ascii;

	auto& gameMode = this->getGameMode();
	if (auto chat = gameMode.chat.get())
	{
		std::string prefix;
		switch(messageType_)
		{
			case LogMessage::Info: prefix = "INFO"; break;
			case LogMessage::Warning: prefix = "WARNING"; break;
			case LogMessage::Error: prefix = "ERROR"; break;
			case LogMessage::FatalError: prefix = "FATAL ERROR"; break;
		}

		message_ = ascii::format("{2}[DEBUG LOG | {0}]: {3}{1}", prefix, message_, colors::Lightslategray, colors::Lightgray);

		auto const& playerPool = this->getGameMode().players.getPool();
		for(auto p : playerPool)
		{
			if (p->receivesServerDebugLog()) {
				chat->messagePlayer(*p, message_);
			}
		}
	}
		
}

#endif

}