#include "TestGMPCH.hpp"

#include <filesystem>

namespace samp = samp_edgengine;
namespace rcio = samp_edgengine::ext::resource_io;

void cmd_TeleportToLocation(samp::CommandInput input_)
{
	auto params = input_.splitParams();
	if (params.size() >= 3)
	{
		samp::math::Vector3f location;
		location.x = samp::text::ascii::convert<float>(params[0]).value();
		location.y = samp::text::ascii::convert<float>(params[1]).value();
		location.z = samp::text::ascii::convert<float>(params[2]).value();
		input_.target.setLocation(location);
		if (params.size() >= 5)
		{
			std::int32_t world		= samp::text::ascii::convert<std::int32_t>(params[3]).value();
			std::int32_t interior	= samp::text::ascii::convert<std::int32_t>(params[4]).value();
			input_.target.setWorld(world);
			input_.target.setInterior(interior);
		}
	}
}

void cmd_SpawnVehicle(samp::CommandInput input_)
{
	auto params = input_.splitParams();
	if (params.size() == 1)
	{
		auto pVeh = GameMode->Map.beginConstruction<samp::Vehicle>();
		pVeh->setModel(samp::Vehicle::findModelBestMatch(params[0]));
		pVeh->setLocation(input_.target.getLocation());
		pVeh->setFacingAngle(input_.target.getFacingAngle());
		pVeh->setInterior(input_.target.getInterior());
		pVeh->setWorld(input_.target.getWorld());
		auto &veh = GameMode->Map.finalizeConstruction(pVeh);
		input_.target.putInVehicle(veh, 0);
	}
}

void cmd_ObjectCount(samp::CommandInput input_)
{
	namespace txt = samp::text::ascii;
	auto tracker = static_cast<samp::default_streamer::PlayerWrapper*>(input_.target.getPlacementTracker());
	GameMode->Chat->messagePlayer(input_.target, txt::compose("You have currently ", tracker->spawnedObjects.size(), " objects shown!"));
}

class MyGameMode
	: public samp::IGameMode
{
public:
	MyGameMode() {	
		this->addPlayerClass(0, { 280, 180, 1010 }, 0, { samp::Weapon(samp::Weapon::Deagle, 999) });
	}
	
	virtual void setup() override
	{
		samp::IGameMode::setup();

		Chat = std::make_unique<samp::DefaultChat>();

		this->setupCommands();
		this->loadObjects();
	}

	void loadObjects()
	{
		namespace fs = std::filesystem;

		for (auto& file : fs::directory_iterator("data/objects"))
		{
			if (fs::is_regular_file(file))
			{
				std::clog << "[i]: Reading file: " << file.path() << std::endl;

				// Open file:
				std::ifstream inputStream{ file };

				auto scene = GameMode->Map.beginConstruction<samp::Scene>();

				// Create XML document:
				rcio::xml::xml_document<> document;

				// Read entire file to the string:
				std::string inputContent;
				{
					char buffer[4 * 1024];
					while (inputStream.read(buffer, sizeof(buffer)))
						inputContent.append(buffer, sizeof(buffer));
					inputContent.append(buffer, inputStream.gcount());
				}
				// Make sure string is null-terminated:
				inputContent.c_str();
				// Parse the document:
				document.parse<0>(inputContent.data());

				if (auto rootNode = document.first_node("MapScene"))
				{
					// Deserialize the scene:
					rcio::XMLSceneDeserializer deserializer{ *scene, *rootNode };
					if (deserializer.deserialize())
					{
						GameMode->Map.finalizeConstruction(scene);
						std::cout << "Read: " << scene->getObjects().size() << " objects!" << std::endl;
					}
				}
				
			}
		}

	}

	virtual void setupEvents() override {
		Server->onPlayerSpawn += { *this, &MyGameMode::whenPlayerSpawns };
	}

	void whenPlayerSpawns(samp::Player & player_)
	{
		player_.setLocation({ 1958.33f, 1343.12f, 15.36f });
		player_.setWorld(0);
		player_.addWeapon(samp::Weapon(samp::Weapon::Deagle, 999));
	}

	void setupCommands()
	{
		Commands.construct<samp::ProcedureCommand>( samp::CmdInvocations{ "tpc" }, cmd_TeleportToLocation );
		Commands.construct<samp::ProcedureCommand>( samp::CmdInvocations{ "v" }, cmd_SpawnVehicle );
		Commands.construct<samp::ProcedureCommand>( samp::CmdInvocations{ "objc" }, cmd_ObjectCount);
	}

	virtual samp::SharedPtr<samp::Player> newPlayerInstance(std::size_t const playerIndex_) const override
	{
		return samp::IGameMode::newPlayerInstance(playerIndex_);
	}
};

samp::GameModeSetupResult SAMPGameModeSetup()
{
	return std::make_unique<MyGameMode>();
}